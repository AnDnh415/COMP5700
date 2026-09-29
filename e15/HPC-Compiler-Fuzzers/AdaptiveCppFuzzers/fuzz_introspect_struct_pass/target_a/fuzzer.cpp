#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

// Isolated createConstantIntArray function
template<unsigned N>
llvm::Constant* createConstantIntArray(const llvm::SmallVector<int, N>& Data, llvm::Module& M) {
  llvm::SmallVector<llvm::Constant*, N> Fields;
  for(int i = 0; i < Data.size(); ++i) {
    Fields.push_back(llvm::Constant::getIntegerValue(
        llvm::IntegerType::getInt32Ty(M.getContext()),
        llvm::APInt{32, static_cast<uint64_t>(Data[i])}));
  }
  return llvm::ConstantArray::get(
      llvm::ArrayType::get(llvm::IntegerType::getInt32Ty(M.getContext()), Fields.size()),
      Fields);
}

// Fuzz target for createConstantIntArray
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Size < sizeof(int))
    return 0;

  // Convert fuzz input into a vector of integers
  llvm::SmallVector<int, 16> FuzzData;
  for (size_t i = 0; i < Size / sizeof(int); ++i) {
    int val = 0;
    std::memcpy(&val, Data + i * sizeof(int), sizeof(int));
    FuzzData.push_back(val);
  }

  // Create an LLVM context and module to hold the constant arrays
  llvm::LLVMContext Context;
  llvm::Module M("FuzzModule", Context);

  // Call the target function
  createConstantIntArray(FuzzData, M);

  return 0;
}