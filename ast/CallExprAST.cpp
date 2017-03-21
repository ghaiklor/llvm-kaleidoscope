#include "./CallExprAST.h"

// Generate LLVM code for function calls
Value *CallExprAST::codegen() {
  Function *CalleeF = TheModule->getFunction(Callee);

  if (!CalleeF) {
    return LogErrorV("Unknown function referenced");
  }

  if (CalleeF->arg_size() != Args.size()) {
    return LogErrorV("Incorrect # arguments passed");
  }

  std::vector<Value *> ArgsV;
  for (unsigned i = 0, e = Args.size(); i != e; i++) {
    ArgsV.push_back(Args[i]->codegen());

    if (!ArgsV.back()) {
      return nullptr;
    }
  }

  return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}
