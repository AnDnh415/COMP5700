#include "hipSYCL/compiler/cbs/VectorizationInfo.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/DerivedTypes.h>

using namespace llvm;
using namespace hipsycl::compiler;



extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size < 3) return 0; // Ensure we have enough data

    LLVMContext Context;
    Module M("test", Context);

    // Create a function
    FunctionType *FTy = FunctionType::get(Type::getVoidTy(Context), false);
    Function *F = Function::Create(FTy, Function::ExternalLinkage, "test", M);

    // Create basic blocks
    BasicBlock *Entry = BasicBlock::Create(Context, "entry", F);
    BasicBlock *Loop = BasicBlock::Create(Context, "loop", F);
    BasicBlock *Exit = BasicBlock::Create(Context, "exit", F);

    // Create a simple loop structure
    BranchInst::Create(Loop, Entry);
    BranchInst::Create(Loop, Exit, ConstantInt::getTrue(Context), Loop);
    ReturnInst::Create(Context, Exit);

    // Create an instruction in the loop
    Instruction *Inst = new AllocaInst(Type::getInt32Ty(Context), 0, "alloca", Loop);

    // Create LoopInfo
    LoopInfo LI;
    LI.analyze(*F);

    // Create VectorizationInfo
    Region R(Entry, Exit);
    VectorizationInfo VI(*F, R);

    // Use the fuzzer data to set some properties
    if (Data[0] & 1) VI.addDivergentLoop(*LI.getLoopFor(Loop));
    if (Data[1] & 1) VI.addDivergentLoopExit(*Exit);

    // Call the function we're fuzzing
    VI.isTemporalDivergent(LI, *Exit, *Inst);

    return 0;
}
