#ifndef __FUNCTION_AST_H__
#define __FUNCTION_AST_H__

#include "ast/PrototypeAST.h"

// Represents a function definition itself
class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body) : Proto(std::move(Proto)), Body(std::move(Body)) {}
  llvm::Function *codegen();
};

#endif
