#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FormatVariadic.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>

#include "hipSYCL/compiler/stdpar/SyncElision.hpp" 
#include "hipSYCL/compiler/cbs/IRUtils.hpp" 
#include "hipSYCL/compiler/cbs/IRUtils.hpp"
#include <llvm/ADT/DenseMap.h>
#include "identifyStoresPotentiallyForStdparArgHandling.cpp"

llvm::Module* createTestModule(llvm::LLVMContext& Context) {
    auto M = std::make_unique<llvm::Module>("test_module", Context);
    llvm::IRBuilder<> Builder(Context);
    auto FuncType = llvm::FunctionType::get(Builder.getVoidTy(), false);
    auto F = llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "test_func", M.get());
    auto BB = llvm::BasicBlock::Create(Context, "entry", F);
    Builder.SetInsertPoint(BB);
    auto Alloca = Builder.CreateAlloca(Builder.getInt32Ty(), nullptr, "a");
    Builder.CreateStore(Builder.getInt32(42), Alloca);
    Builder.CreateRetVoid();

    if (verifyModule(*M, &llvm::errs())) {
        llvm::errs() << "Error: Invalid module generated\n";
        return nullptr;
    }

    return M.release();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size) {
    using namespace llvm;
    if (Size < 3) return 0;
    llvm::LLVMContext Context;
    auto M = createTestModule(Context);
    if (!M) return 0;

    /*
    * the elemnts of these iterable data structs will be used
    * by the actual target as arguements
    */

    llvm::SmallVector<Instruction*, 16> StdparCallPositions;
    llvm::SmallPtrSet<Function*, 16> StdparFunctions;
    for (auto& F : M->functions()) {
        if (F.getName() == "test_func") {
            StdparCallPositions.push_back(&*F.begin()->begin());
            StdparFunctions.insert(&F);
        }
    }

    using InstToInstListMapT = llvm::SmallDenseMap<llvm::Instruction *, llvm::SmallVector<llvm::Instruction *, 16>>;
    InstToInstListMapT Out;
    if (Data[0] & 1) StdparFunctions.insert(&*M->getFunctionList().begin());
    if (Data[1] & 1) StdparCallPositions.push_back(&*M->getFunctionList().begin()->begin()->begin());
    identifyStoresPotentiallyForStdparArgHandling(StdparCallPositions, StdparFunctions, Out);
    delete M;
    return 0;
}

