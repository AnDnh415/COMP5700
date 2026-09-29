#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>

// The isolated function we're going to fuzz.
enum class TypeKind { Other = 0, Pointer = 1, IntegerType = 2, FloatType = 3 };

TypeKind getTypeKind(llvm::Type* T) {
  if (T->isPointerTy())
    return TypeKind::Pointer;
  else if (T->isFloatingPointTy())
    return TypeKind::FloatType;
  else if (T->isIntegerTy())
    return TypeKind::IntegerType;
  return TypeKind::Other;
}

// Helper function to create various LLVM types from the fuzz input.
llvm::Type* createTypeFromData(const uint8_t *Data, size_t Size, llvm::LLVMContext &Context) {
  if (Size == 0)
    return llvm::Type::getVoidTy(Context); // Return a simple void type for empty input.

  switch (Data[0] % 6) {
    case 0: return llvm::Type::getInt32Ty(Context);  // Integer type
    case 1: return llvm::Type::getFloatTy(Context);  // Float type
    case 2: return llvm::PointerType::getUnqual(llvm::Type::getDoubleTy(Context)); // Pointer to double
    case 3: return llvm::ArrayType::get(llvm::Type::getInt8Ty(Context), 16); // Array type
    case 4: return llvm::StructType::create(Context, "TestStruct"); // Struct type
    case 5: return llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), {}, false); // Function type
    default: return llvm::Type::getInt1Ty(Context);  // Default fallback type
  }
}

// Fuzz target for getTypeKind
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  llvm::LLVMContext Context;
  
  // Create an LLVM type from the fuzz input.
  llvm::Type* TestType = createTypeFromData(Data, Size, Context);
  
  // Call the target function.
  getTypeKind(TestType);
  
  return 0;
}