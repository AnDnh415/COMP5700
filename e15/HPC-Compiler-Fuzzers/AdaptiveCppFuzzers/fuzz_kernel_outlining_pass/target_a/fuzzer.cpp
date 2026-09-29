#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/GlobalAlias.h>
#include <llvm/IR/Constants.h>
#include <cstdint>
#include <cstdlib>
#include<iostream>

bool isUsedInFunctions(llvm::SmallPtrSet<llvm::User*, 16>& VisitedUsers, llvm::User* User) {
  //std::cout << "Here" << std::endl; //DEBUG
  if(llvm::isa<llvm::Function>(User))
      return true;
  //std::cout << "Passed First check" << std::endl; //DEBUG
  if(llvm::Instruction* I = llvm::dyn_cast<llvm::Instruction>(User)){
    if(I->getFunction())
      return true;
  }
  //std::cout << "Passed Second check" << std::endl; //DEBUG
  if(VisitedUsers.contains(User))
    return false;
  VisitedUsers.insert(User);

  for(auto* U : User->users()) {
    if(isUsedInFunctions(VisitedUsers, U))
      return true;
  }
  return false;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) 
{
  if (Size >= sizeof(int)*2) 
  {
    llvm::LLVMContext Context;
    llvm::Module M("fuzz_module", Context);
    llvm::FunctionType *FuncType = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), false);
    llvm::Function *F = llvm::Function::Create(FuncType, llvm::GlobalValue::ExternalLinkage, "testFunction", M);
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(Context, "entry", F);
    llvm::Instruction *Inst1 = llvm::BinaryOperator::CreateAdd(
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 42),
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 13),
        "add", BB
    );
    llvm::SmallPtrSet<llvm::User*, 16> VisitedUsers;
    int a = *reinterpret_cast<const int*>(Data + 0);
    //std::cout << "a : " << a << std::endl; //DEBUG
    int b = *reinterpret_cast<const int*>(Data + sizeof(int));
    //std::cout << "b : " << b << std::endl; //DEBUG
    llvm::Instruction *Inst2 = llvm::BinaryOperator::CreateAdd(
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), a),
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), b),
      "add", BB
    );
    VisitedUsers.insert(Inst1);
    VisitedUsers.insert(Inst2);
    isUsedInFunctions(VisitedUsers, Inst2);
  }
  return 0;
}