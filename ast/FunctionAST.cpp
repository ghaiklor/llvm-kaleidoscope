#include "./FunctionAST.h"

// Generates LLVM code for functions declarations
Function *FunctionAST::codegen() {
  Function *TheFunction = TheModule->getFunction(Proto->getName());

  if (!TheFunction) {
    TheFunction = Proto->codegen();
  }

  if (!TheFunction) {
    return nullptr;
  }

  BasicBlock *BB = BasicBlock::Create(TheContext, "entry", TheFunction);
  Builder.SetInsertPoint(BB);
  NamedValues.clear();
  for (auto &Arg : TheFunction->args()) {
    NamedValues[Arg.getName()] = &Arg;
  }

  if (Value *RetVal = Body->codegen()) {
    Builder.CreateRet(RetVal);
    verifyFunction(*TheFunction);

    return TheFunction;
  }

  TheFunction->eraseFromParent();
  return nullptr;
}
