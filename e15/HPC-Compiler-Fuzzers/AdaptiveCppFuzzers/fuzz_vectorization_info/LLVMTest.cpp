
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/ValueHandle.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

int main()
{
    std::cout << "testing" << std::endl;
    llvm::LLVMContext context;
    llvm::Module module("example", context);

    // Create a function type: void function()
    llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);

    // Create a function with the specified type
    llvm::Function *function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "exampleFunction", module);

    // Create a basic block and attach it to the function
    llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(context, "entry", function);

    // Create an IRBuilder and set the insertion point to the basic block
    llvm::IRBuilder<> builder(basicBlock);

    // Create a constant integer value (e.g., int32 42)
    llvm::Value *constValue = llvm::ConstantInt::get(context, llvm::APInt(32, 42));

    // Use the constant value in an instruction (e.g., add instruction)
    llvm::Value *result = builder.CreateAdd(constValue, constValue, "addtmp");

    // Finish the basic block with a return instruction
    builder.CreateRetVoid();

    // Print the module's IR to stdout
    module.print(llvm::outs(), nullptr);
    std::cout << "done testing" << std::endl;
    return 0;
}
