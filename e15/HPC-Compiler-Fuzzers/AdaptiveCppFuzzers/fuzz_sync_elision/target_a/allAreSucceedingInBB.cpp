/*
 * This file is part of AdaptiveCpp, an implementation of SYCL and C++ standard
 * parallelism for CPUs and GPUs.
 *
 * Copyright The AdaptiveCpp Contributors
 *
 * AdaptiveCpp is released under the BSD 2-Clause "Simplified" License.
 * See file LICENSE in the project root for full license details.
 */
// SPDX-License-Identifier: BSD-2-Clause
#include "hipSYCL/compiler/stdpar/SyncElision.hpp"
#include "hipSYCL/compiler/cbs/IRUtils.hpp"


#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Attributes.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Instruction.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/PassManager.h>


template <class Handler>
bool descendInstructionUseTree(llvm::Instruction *I, Handler &&H,
                               llvm::Instruction *Parent = nullptr) {
  if(H(I, Parent)) {
    for(auto* U : I->users()) {
      if(auto* UI = llvm::dyn_cast<llvm::Instruction>(U)) {
        if(!descendInstructionUseTree(UI, H, I))
          return false;
      } else {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

using InstToInstListMapT =
    llvm::SmallDenseMap<llvm::Instruction *, llvm::SmallVector<llvm::Instruction *, 16>>;

// Identifies store instructions that might be related for argument handling:
// We identify these instructions by looking for allocas in the function. If that alloca
// is only used by getelementptr, stores, and calls to stdpar functions, chances are
// these instructions are only relevant for constructing stdpar arguments.
//
// The result is a map from encountered store instructions to other instructions referencing the same
// memory.
void identifyStoresPotentiallyForStdparArgHandling(
    llvm::Function *F, const llvm::SmallPtrSet<llvm::Function *, 16> &StdparFunctions,
    InstToInstListMapT &Out) {
      //std::cout<<"starting"<< std::endl;
  for(auto& BB : *F) {
    for(auto& I : BB) {
      if(llvm::isa<llvm::AllocaInst>(&I)) {
        llvm::SmallVector<llvm::Instruction*, 16> Users;

        bool onlyUsedInAllowedInstructions = descendInstructionUseTree(
            &I, [&](llvm::Instruction *Current, llvm::Instruction *Parent) {
              if (llvm::isa<llvm::AllocaInst>(Current) ||
                  llvm::isa<llvm::GetElementPtrInst>(Current)) {
                Users.push_back(Current);
                return true;
              } else if(auto *SI = llvm::dyn_cast<llvm::StoreInst>(Current)) {
                // For store instructions, we enforce that the previous instruction in
                // the use chain must be the pointer operand, not the value operand.
                if(SI->getValueOperand() != Parent) {
                  Users.push_back(Current);
                  return true;
                }
              } else if (auto *CB = llvm::dyn_cast<llvm::CallBase>(Current)) {
                if (StdparFunctions.contains(CB->getCalledFunction())) {
                  Users.push_back(Current);
                  return true;
                } else if(CB->getCalledFunction()->getName().startswith("llvm.lifetime")) {
                  return true;
                }
              }

              return false;
            });

        if(onlyUsedInAllowedInstructions) {
          for(auto* U: Users) {
            if(auto* SI = llvm::dyn_cast<llvm::StoreInst>(U)) {
              for(auto* U : Users) {
                Out[SI].push_back(U);
              }  
            }
          }
        }
      }
    }
  }
}

void identifyStoresPotentiallyForStdparArgHandling(
    const llvm::SmallVector<llvm::Instruction *, 16> &StdparCallPositions,
    const llvm::SmallPtrSet<llvm::Function *, 16> &StdparFunctions,
    InstToInstListMapT &Out) {
      
 // std::cout<<"inside target"<< std::endl;
  llvm::SmallPtrSet<llvm::Function*, 16> InvolvedFunctions;

  for(auto* I : StdparCallPositions) {
    if(I) {
      InvolvedFunctions.insert(I->getParent()->getParent());
    }
  }

  for(auto* F: InvolvedFunctions) {
    identifyStoresPotentiallyForStdparArgHandling(F, StdparFunctions, Out);
  }
}

bool instructionAccessesMemory(llvm::Instruction* I) {
  if (llvm::isa<llvm::StoreInst>(I) || llvm::isa<llvm::LoadInst>(I) ||
      llvm::isa<llvm::AtomicRMWInst>(I) || llvm::isa<llvm::AtomicCmpXchgInst>(I) ||
      llvm::isa<llvm::FenceInst>(I))
    return true;

  return false;
}

bool functionDoesNotAccessMemory(llvm::Function* F){
  if(!F)
    return true;
  if(F->isIntrinsic()) {
    if(F->getName().starts_with("llvm.lifetime")){
      return true;
    }
  }
  // We could improve this logic massively: E.g. a function which does not have ptr arguments,
  // does not have gobal variable users, contains no inttoptr instructions, and only calls functions
  // which satisfy these criteria could be assumed to not access memory.
  return false;
}

// returns whether To is in the same BB as From, and succeeds it in the instruction list.
bool isSucceedingInBB(llvm::Instruction* From, llvm::Instruction* To) {
  if(From->getParent() == To->getParent()) {
    for(auto* I = From; I != nullptr; I = I->getNextNonDebugInstruction()) {
      if(I == To)
        return true;
    }
  } 
  return false;
}

template <unsigned N>
bool allAreSucceedingInBB(llvm::Instruction* From,
                          const llvm::SmallVector<llvm::Instruction *, N> &To) {
  for(auto* I : To) {
    if(!isSucceedingInBB(From, I))
      return false;
  }
  return true;
}

constexpr const char* BarrierBuiltinName = "__acpp_stdpar_optional_barrier";
constexpr const char* EntrypointMarker = "hipsycl_stdpar_entrypoint";

template<class Handler>
void forEachStdparFunction(llvm::Module& M, Handler&& H){
  hipsycl::compiler::utils::findFunctionsWithStringAnnotations(M,  [&](llvm::Function* F, llvm::StringRef Annotation){
    if(F) {
      if(Annotation.compare(EntrypointMarker) == 0) {
        H(F);
      }
    }
  });
}

template <class Handler>
void forEachReachableInstructionRequiringSync(
    llvm::Instruction *Start, const llvm::SmallPtrSet<llvm::Function *, 16> &StdparFunctions,
    const InstToInstListMapT& PotentialStoresForStdparArgs,
    llvm::SmallPtrSet<llvm::BasicBlock*, 16> &CompletelyVisitedBlocks,
    Handler &&H) {

  if(!Start)
    return;

  llvm::Instruction* Current = Start;
  if(CompletelyVisitedBlocks.contains(Current->getParent())) {
    return;
  }

  llvm::Instruction* FirstInst = &(*Current->getParent()->getFirstInsertionPt());
  if(Current == FirstInst) {
    CompletelyVisitedBlocks.insert(Current->getParent());
  }

  while(Current) {
    if(auto* CB = llvm::dyn_cast<llvm::CallBase>(Current)) {
      llvm::Function* CalledF = CB->getCalledFunction();
      if(CalledF->getName().equals(BarrierBuiltinName)) {
        // basic block already contains barrier; nothing to do
        return;
      }

      // If we have found a call to an stdpar function, we can skip it --
      // after all, the whole point is to not sync after every stdpar call.
      // For all other calls, we need a sync because we currently
      // do not take control flow beyond our own function into account.
      // We can also safely ignore functions for which we know that they
      // do not access memory
      bool CanSkipFunctionCall =
          StdparFunctions.contains(CalledF) || functionDoesNotAccessMemory(CalledF);

      if(!CanSkipFunctionCall) {
        H(Current);
        return;
      }
    } else if(instructionAccessesMemory(Current)) {
      bool isSkippableStore = false;
      if(llvm::isa<llvm::StoreInst>(Current)) {
        // Check if the store is perhaps only used to setup arguments of stdpar calls
        // (e.g. to assemble kernel lambdas)
        auto It = PotentialStoresForStdparArgs.find(Current);
        if(It != PotentialStoresForStdparArgs.end()) {
          // Store is skippable, if the referenced memory is used by stdpar function calls
          // which succeed the store in the control flow.
          llvm::SmallVector<llvm::Instruction*, 16> StdparCallsUsingMemory;
          for(auto* I : It->getSecond()) {
            if(auto *CB = llvm::dyn_cast<llvm::CallBase>(I)){
              if(StdparFunctions.contains(CB->getCalledFunction())) {
                StdparCallsUsingMemory.push_back(CB);
              }
            }
          }
          if(allAreSucceedingInBB(Current, StdparCallsUsingMemory)) {
            isSkippableStore = true;
          }
        }
      }

      if(!isSkippableStore) {
        H(Current);
        return;
      } else {
        HIPSYCL_DEBUG_INFO
            << "[stdpar] SyncElision: Detected store that does not block barrier movement\n";
      }
    } else if(Current->isTerminator()){
      // If this terminator causes control flow to exit from this function, we need
      // to insert synchronization.
      // TODO: Look again at exception handling instructions in more detail
      if (llvm::isa<llvm::ReturnInst>(Current) || llvm::isa<llvm::InvokeInst>(Current) ||
          llvm::isa<llvm::CallBrInst>(Current) || llvm::isa<llvm::ResumeInst>(Current)) {
        H(Current);
        return;
      }
    }
    Current = Current->getNextNonDebugInstruction();
  }
  // We have reached the end of this BB - so we need to look
  // at all its successors in the CFG
  llvm::BasicBlock* BB = Start->getParent();
  for(int i = 0; i < BB->getTerminator()->getNumSuccessors(); ++i) {
    llvm::BasicBlock* Successor = BB->getTerminator()->getSuccessor(i);
    if(Successor->size() > 0) {
      llvm::Instruction* FirstI = &(*Successor->getFirstInsertionPt());
      forEachReachableInstructionRequiringSync(
          FirstI, StdparFunctions, PotentialStoresForStdparArgs, CompletelyVisitedBlocks, H);
    }
  }
}