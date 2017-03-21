// lexer headers
#include "./lexer/lexer.h"
#include "./lexer/token.h"

// AST headers
#include "./ast/BinaryExprAST.h"
#include "./ast/CallExprAST.h"
#include "./ast/ExprAST.h"
#include "./ast/FunctionAST.h"
#include "./ast/NumberExprAST.h"
#include "./ast/PrototypeAST.h"
#include "./ast/VariableExprAST.h"

// LLVM headers
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

// stdlib headers
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

static std::map<char, int> BinopPrecedence;
static int GetTokPrecedence() {
  if (!isascii(CurTok)) {
    return -1;
  }

  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0) return -1;

  return TokPrec;
}

// This is an object that owns LLVM core data structures
static LLVMContext TheContext;

// This is a helper object that makes easy to generate LLVM instructions
static IRBuilder<> Builder(TheContext);

// This is an LLVM construct that contains functions and global variables
static std::unique_ptr<Module> TheModule;

// This map keeps track of which values are defined in the current scope
static std::map<std::string, Value *> NamedValues;

// Some helpers for error handling
std::unique_ptr<ExprAST> LogError(const char *Str) {
  fprintf(stderr, "LogError: %s\n", Str);
  return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
  LogError(Str);
  return nullptr;
}

Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

static std::unique_ptr<ExprAST> ParseExpression();

// This routine expects to be called when the current token is a tok_number
// It takes the current number value and creates a NumberExprAST node
static std::unique_ptr<ExprAST> ParseNumberExpr() {
  auto Result = llvm::make_unique<NumberExprAST>(NumVal);
  getNextToken();
  return std::move(Result);
}

// This routine parses expressions in "(" and ")" characters
static std::unique_ptr<ExprAST> ParseParenExpr() {
  getNextToken();

  auto V = ParseExpression();
  if (!V) {
    return nullptr;
  }

  if (CurTok != ')') {
    return LogError("Expected )");
  }

  getNextToken();
  return V;
}

// This routine expects to be called when current token is tok_identifier
static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
  std::string IdName = IdentifierStr;

  getNextToken();

  if (CurTok != '(') {
    return llvm::make_unique<VariableExprAST>(IdName);
  }

  getNextToken();
  std::vector<std::unique_ptr<ExprAST>> Args;
  if (CurTok != ')') {
    while (true) {
      if (auto Arg = ParseExpression()) {
        Args.push_back(std::move(Arg));
      } else {
        return nullptr;
      }

      if (CurTok == ')') {
        break;
      }

      if (CurTok != ',') {
        return LogError("Expected ')' or ',' in argument list");
      }

      getNextToken();
    }
  }

  getNextToken();

  return llvm::make_unique<CallExprAST>(IdName, std::move(Args));
}

static std::unique_ptr<ExprAST> ParsePrimary() {
  switch (CurTok) {
    default:
    return LogError("Unknown token when expecting an expression");
    case tok_identifier:
    return ParseIdentifierExpr();
    case tok_number:
    return ParseNumberExpr();
    case '(':
    return ParseParenExpr();
  }
}

static std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS) {
  while (true) {
    int TokPrec = GetTokPrecedence();

    if (TokPrec < ExprPrec) {
      return LHS;
    }

    int BinOp = CurTok;
    getNextToken();

    auto RHS = ParsePrimary();
    if (!RHS) {
      return nullptr;
    }

    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec) {
      RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
      if (!RHS) {
        return nullptr;
      }
    }

    LHS = llvm::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
  }
}

static std::unique_ptr<ExprAST> ParseExpression() {
  auto LHS = ParsePrimary();

  if (!LHS) {
    return nullptr;
  }

  return ParseBinOpRHS(0, std::move(LHS));
}

static std::unique_ptr<PrototypeAST> ParsePrototype() {
  if (CurTok != tok_identifier) {
    return LogErrorP("Expected function name in prototype");
  }

  std::string FnName = IdentifierStr;
  getNextToken();

  if (CurTok != '(') {
    return LogErrorP("Expected '(' in prototype");
  }

  std::vector<std::string> ArgNames;
  while (getNextToken() == tok_identifier) {
    ArgNames.push_back(IdentifierStr);
  }

  if (CurTok != ')') {
    return LogErrorP("Expected ')' in prototype");
  }

  getNextToken();

  return llvm::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

static std::unique_ptr<FunctionAST> ParseDefinition() {
  getNextToken();

  auto Proto = ParsePrototype();
  if (!Proto) {
    return nullptr;
  }

  if (auto E = ParseExpression()) {
    return llvm::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }

  return nullptr;
}

static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto E = ParseExpression()) {
    auto Proto = llvm::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
    return llvm::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }

  return nullptr;
}

static std::unique_ptr<PrototypeAST> ParseExtern() {
  getNextToken();
  return ParsePrototype();
}

static void HandleDefinition() {
  if (auto FnAST = ParseDefinition()) {
    if (auto *FnIR = FnAST->codegen()) {
      fprintf(stderr, "Read function definition:");
      FnIR->print(errs());
      fprintf(stderr, "\n");
    }
  } else {
    getNextToken();
  }
}

static void HandleExtern() {
  if (auto ProtoAST = ParseExtern()) {
    if (auto *FnIR = ProtoAST->codegen()) {
      fprintf(stderr, "Read extern:");
      FnIR->print(errs());
      fprintf(stderr, "\n");
    }
  } else {
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  if (auto FnAST = ParseTopLevelExpr()) {
    if (auto *FnIR = FnAST->codegen()) {
      fprintf(stderr, "Read top-level expression:");
      FnIR->print(errs());
      fprintf(stderr, "\n");
    }
  } else {
    getNextToken();
  }
}

static void MainLoop() {
  while (true) {
    fprintf(stderr, "ready> ");

    switch (CurTok) {
      case tok_eof:
      return;
      case ';':
      getNextToken();
      break;
      case tok_def:
      HandleDefinition();
      break;
      case tok_extern:
      HandleExtern();
      break;
      default:
      HandleTopLevelExpression();
      break;
    }
  }
}

int main() {
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;

  fprintf(stderr, "ready> ");
  getNextToken();

  TheModule = llvm::make_unique<Module>("My awesome JIT", TheContext);

  MainLoop();

  TheModule->print(errs(), nullptr);

  return 0;
}
