#include "./PrototypeAST.h"

// Generates LLVM code for externals calls
Function *PrototypeAST::codegen() {
  std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(TheContext));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);
  Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

  unsigned Idx = 0;
  for (auto &Arg : F->args()) {
    Arg.setName(Args[Idx++]);
  }

  return F;
}
