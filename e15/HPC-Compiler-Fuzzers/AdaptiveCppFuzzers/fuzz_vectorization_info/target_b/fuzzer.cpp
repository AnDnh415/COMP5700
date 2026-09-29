#include <cstdint>
#include <cstddef>
#include <vector>
#include <random>
//llvm
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Transforms/Utils/LoopUtils.h>
#include <llvm/IR/LegacyPassManager.h>
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
//acpp
#include "hipSYCL/compiler/cbs/VectorizationInfo.hpp"

using namespace llvm;

std::random_device rd;
std::mt19937 gen(rd());

bool randomBool() { return std::uniform_int_distribution<>(0, 1)(gen); }

/*
* run using -detect_leaks=0
* this fuzzer finds a memory leak in void llvm::LoopBase<llvm::BasicBlock, llvm::Loop>::addBasicBlockToLoop(llvm::BasicBlock *NewBB, llvm::LoopInfo::BaseT &LI)
*/
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size) 
{
  //if (Size < sizeof(int)*2) {return 0;}
  llvm::LLVMContext Context;
  llvm::Module *module = new Module("test_module", Context);
  //llvm::IRBuilder<> builder(Context);
  //func
  std::vector<llvm::Type *> args;
  args.push_back(llvm::Type::getInt32Ty(Context));
  args.push_back(llvm::Type::getInt32Ty(Context));
  args.push_back(llvm::Type::getInt8Ty(Context));
  llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), args, false);
  llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "testFunc", module);
  
  llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(Context, "entryBB", func);
  
  llvm::LoopInfo LI;
  llvm::Value *val;
  Loop *ParentLoop = LI.AllocateLoop();
  Loop *ChildLoop = nullptr;
  
  ParentLoop->addBasicBlockToLoop(entryBB, LI);
  if (randomBool()) 
  {
    val = new AllocaInst(Type::getFloatTy(Context), 0, "llvm::val", entryBB);
    ChildLoop = LI.AllocateLoop();
    ChildLoop->setParentLoop(ParentLoop);
  }
  else
  {
    val = UndefValue::get(Type::getFloatTy(Context));
  }

  if(ChildLoop)
  {
    LI.addTopLevelLoop(ChildLoop); 
    hipsycl::compiler::LoopRegion loopRgn(*ChildLoop);
    hipsycl::compiler::Region rgn(loopRgn);
    hipsycl::compiler::VectorizationInfo vctrznObj(*func, rgn);
    vctrznObj.isTemporalDivergent(LI, *entryBB, *val);
  }
  else 
  {
    LI.addTopLevelLoop(ParentLoop); 
    hipsycl::compiler::LoopRegion loopRgn(*ParentLoop);
    hipsycl::compiler::Region rgn(loopRgn);
    hipsycl::compiler::VectorizationInfo vctrznObj(*func, rgn);
    vctrznObj.isTemporalDivergent(LI, *entryBB, *val);
  }

  /*
  std::vector<llvm::Type *> args;
  args.push_back(llvm::Type::getInt32Ty(Context));
  args.push_back(llvm::Type::getInt32Ty(Context));
  args.push_back(llvm::Type::getInt8Ty(Context));
  llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), args, false);
  llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "testFunc", module);
  llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(Context, "entryBB", func);
  builder.SetInsertPoint(entryBB);
  for(auto &arg : func->args()) {builder.CreateAlloca(arg.getType());}
  ///
  int fuzzedLb = *reinterpret_cast<const int*>(Data);
  int fuzzedUb = *reinterpret_cast<const int*>(Data + sizeof(int));
  ///
  Value *lb = builder.getInt32(fuzzedLb);
  Value *ub = builder.getInt32(fuzzedUb);
  Value *cntr = builder.CreateAlloca(builder.getInt32Ty(), nullptr, "counter");
  builder.CreateStore(lb, cntr);
  BasicBlock *loopBB = BasicBlock::Create(Context, "loop", func);
  BasicBlock *afterBB = BasicBlock::Create(Context, "afterloop", func);
  builder.CreateBr(loopBB);
  builder.SetInsertPoint(loopBB);
  Value *ldCntr = builder.CreateLoad(builder.getInt32Ty(), cntr, "counterVal");
  Value *Cond = builder.CreateICmpSLT(ldCntr, ub, "loopcond");
  builder.CreateCondBr(Cond, loopBB, afterBB);
  Value *increment = builder.CreateAdd(ldCntr, builder.getInt32(1), "increment");
  builder.CreateStore(increment, cntr);
  builder.SetInsertPoint(afterBB);
  builder.CreateRetVoid();
  llvm::legacy::FunctionPassManager FPM(module);
  auto *LoopInfoPass = new LoopInfoWrapperPass();
  FPM.add(LoopInfoPass);
  FPM.run(*func);
  LoopInfo &LI = LoopInfoPass->getLoopInfo();
  */

  // hipsycl::compiler::LoopRegion loopRgn(*LI.getLoopFor(loopBB));
  // hipsycl::compiler::Region rgn(loopRgn);
  // hipsycl::compiler::VectorizationInfo vctrznObj(*func, rgn);

  //vctrznObj.isTemporalDivergent(LI, *loopBB, *func);
  //delete module;
  return 0;
}
