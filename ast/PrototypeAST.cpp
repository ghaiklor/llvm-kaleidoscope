#include "PrototypeAST.h"

// Generates LLVM code for externals calls
llvm::Function *PrototypeAST::codegen() {
  std::vector<llvm::Type *> Doubles(Args.size(), Type::getDoubleTy(TheContext));
  llvm::FunctionType *FT = FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);
  llvm::Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

  unsigned Idx = 0;
  for (auto &Arg : F->args()) {
    Arg.setName(Args[Idx++]);
  }

  return F;
}
