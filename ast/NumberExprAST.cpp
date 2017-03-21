#include "NumberExprAST.h"

// Generate LLVM code for numeric literals
llvm::Value *NumberExprAST::codegen() {
  return ConstantFP::get(TheContext, APFloat(Val));
}
