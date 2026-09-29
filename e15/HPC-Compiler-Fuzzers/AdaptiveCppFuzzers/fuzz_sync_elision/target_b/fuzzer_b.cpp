/*
The fuzzer made by this harness of SyncElision::identifyStoresPotentiallyForStdparArgHandling makes better use of the supplied fuzzing inputs
yet this one gives less cov (102 vs 114) we can run both for a while and see which one performs better in the long run
*/
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

using namespace llvm;
using namespace hipsycl::compiler;

llvm::Module* createTestModule(llvm::LLVMContext& Context, const uint8_t *Data, std::size_t Size) {
    auto M = std::make_unique<llvm::Module>("test_module", Context);
    llvm::IRBuilder<> Builder(Context);
    auto FuncType = llvm::FunctionType::get(Builder.getVoidTy(), false);
    auto F = llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "test_func", M.get());
    auto BB = llvm::BasicBlock::Create(Context, "entry", F);
    Builder.SetInsertPoint(BB);
    auto Alloca = Builder.CreateAlloca(Builder.getInt32Ty(), nullptr, "a");
    if (Size > 0) {
        int32_t value = static_cast<int32_t>(Data[0]);
        Builder.CreateStore(Builder.getInt32(value), Alloca);
    }
    Builder.CreateRetVoid();
    if (verifyModule(*M, &llvm::errs())) {
        llvm::errs() << "Error: Invalid module generated\n";
        return nullptr;
    }
    return M.release();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size) {
    if (Size == 0) return 0;
    LLVMContext Context;
    auto M = createTestModule(Context, Data, Size);
    if (!M) return 0;

    SmallVector<Instruction*, 16> StdparCallPositions;
    SmallPtrSet<Function*, 16> StdparFunctions;
    for (auto& F : M->functions()) {
        if (F.getName() == "test_func") {
            StdparCallPositions.push_back(&*F.begin()->begin());
            StdparFunctions.insert(&F);
        }
    }
    using InstToInstListMapT = SmallDenseMap<Instruction *, SmallVector<Instruction *, 16>>;
    InstToInstListMapT Out;
    identifyStoresPotentiallyForStdparArgHandling(StdparCallPositions, StdparFunctions, Out);
    delete M;
    return 0;
}

