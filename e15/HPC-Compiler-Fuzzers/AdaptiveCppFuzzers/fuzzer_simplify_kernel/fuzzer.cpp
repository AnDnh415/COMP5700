#include "SimplifyKernel.cpp"
#include "hipSYCL/compiler/cbs/SimplifyKernel.hpp"

#include "hipSYCL/compiler/cbs/IRUtils.hpp"
#include "hipSYCL/compiler/cbs/SplitterAnnotationAnalysis.hpp"

#include "hipSYCL/common/debug.hpp"

#include <llvm/Analysis/AssumptionCache.h>
#include <llvm/Analysis/InstructionSimplify.h>
#include <llvm/Analysis/MemorySSA.h>
#include <llvm/Analysis/MemorySSAUpdater.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Transforms/Utils/LoopRotationUtils.h>
#include <llvm/Transforms/Utils/LoopUtils.h>
#include <llvm/Passes/PassBuilder.h>

/*
// Helper function to create a simple module and function with some instructions
llvm::Module* createTestModule(llvm::LLVMContext& Context, std::size_t Size, uint8_t Data) {
    auto M = std::make_unique<llvm::Module>("test_module", Context);
    llvm::IRBuilder<> Builder(Context);

    // function type and a function
    auto FuncType = llvm::FunctionType::get(Builder.getPtrTy(Size), false);
    auto F = llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, "test_func", M.get());

    // Create a basic block and prepare to insert some instructions
    auto BB = llvm::BasicBlock::Create(Context, "entry", F);
    Builder.SetInsertPoint(BB);
    std::string val = reinterpret_cast<const char*>(Data);
    int num_F = 5;
    for(int i = 0; i < num_F; i++)
    {
        // Create an alloca instruction
        auto Alloca = Builder.CreateAlloca(Builder.getInt32Ty(), nullptr, val);
        // Create some instructions that use the alloca
        Builder.CreateStore(Builder.getInt32(42), Alloca);  
    }

    //llvm::Value* LeftOperand = llvm::ConstantInt::get(Context, llvm::APInt(4096, Data[0]));
    //llvm::Value* RightOperand = llvm::ConstantInt::get(Context, llvm::APInt(4096, 786));
    //llvm::Instruction* from_inst = Builder.CreateAddrSpaceCast(LeftOperand, RightOperand);
    
    //insert inst at end of BB

    //llvm::SmallVector<llvm::Instruction*, Data> StdparCallPositions;
    //llvm::SmallPtrSet<Function*, 16> StdparFunctions;

    // Terminate the basic block
    Builder.CreateRetVoid();

    // Validate the module
    if (verifyModule(*M, &llvm::errs())) {
        llvm::errs() << "Error: Invalid module generated\n";
        return nullptr;
    }

    return M.release();
}
*/
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *Data, size_t Size) {
    if (Size < 8) return 0;  // Require at least 8 bytes for more diverse input

    llvm::LLVMContext context;
    auto m = std::make_unique<llvm::Module>("test_module", context);
    llvm::IRBuilder<> builder(context);

    // Create function type and function
    auto funcType = llvm::FunctionType::get(builder.getDoubleTy(), {builder.getDoubleTy()}, false);
    auto f = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "test_func", m.get());
    auto bb = llvm::BasicBlock::Create(context, "entry", f);
    builder.SetInsertPoint(bb);

    // Use more input data to create diverse IR
    double inputVal1 = *reinterpret_cast<const double*>(Data);
    double inputVal2 = inputVal1 * 2.0;  // Some arbitrary computation

    // Create alloca and use it
    auto alloca = builder.CreateAlloca(builder.getDoubleTy(), nullptr, "test_alloc");
    builder.CreateStore(llvm::ConstantFP::get(builder.getDoubleTy(), inputVal1), alloca);
    
    // Create some instructions using the alloca and function parameter
    auto load = builder.CreateLoad(builder.getDoubleTy(), alloca);
    auto param = &*f->arg_begin();
    auto add = builder.CreateFAdd(load, param);
    auto mul = builder.CreateFMul(add, llvm::ConstantFP::get(builder.getDoubleTy(), inputVal2));
    
    // Create a return instruction
    builder.CreateRet(mul);

    // Set up LLVM's pass infrastructure
    llvm::FunctionAnalysisManager FAM;
    llvm::PassBuilder PB;
    PB.registerFunctionAnalyses(FAM);

    // Create and run SimplifyKernelPass
    hipsycl::compiler::SimplifyKernelPass pass;
    for (auto& F : m->functions()) {
        if (F.getName() == "test_func") {
            pass.run(F, FAM);
        }
    }

    return 0;
}



