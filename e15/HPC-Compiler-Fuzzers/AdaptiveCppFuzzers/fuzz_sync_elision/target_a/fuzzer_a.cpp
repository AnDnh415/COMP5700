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
#include "allAreSucceedingInBB.cpp"

// Helper function to create a simple module and function with some instructions
llvm::Module* createTestModule(llvm::LLVMContext& Context, std::size_t Size, const uint8_t *Data) {
    auto M = std::make_unique<llvm::Module>("test_module", Context);
    llvm::IRBuilder<> Builder(Context);

    auto FuncType = llvm::FunctionType::get(Builder.getVoidTy(), false);
    auto F = llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "test_func", M.get());

    auto BB = llvm::BasicBlock::Create(Context, "entry", F);
    Builder.SetInsertPoint(BB);

    /*
    * Add 5 Alloca Instructions to this BasicBlock
    * each Instruction uses Alloca obj with a fuzzed name
    */
    std::string val(reinterpret_cast<const char*>(Data), Size);
    int num_F = 5;
    for(int i = 0; i < num_F; i++)
    {
        auto Alloca = Builder.CreateAlloca(Builder.getInt32Ty(), nullptr, val);
        Builder.CreateStore(Builder.getInt32(42), Alloca);  
    }

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
    auto M = createTestModule(Context, Size, Data);
    if (!M) return 0;

    /*
    * The elements of instVec iterable data struct will be used
    * by the allAreSucceedingInBB (target) as arguements
    */
    llvm::SmallVector<Instruction*, 16> instVec;
    llvm::Instruction* from;
    for (auto& F : M->functions()) {
        if (F.getName() == "test_func") {
            instVec.push_back(&*F.begin()->begin());
            if(!from) {from = &*F.begin()->begin();}
        }
    }
    allAreSucceedingInBB(from, instVec);
    delete M;
    return 0;
}

