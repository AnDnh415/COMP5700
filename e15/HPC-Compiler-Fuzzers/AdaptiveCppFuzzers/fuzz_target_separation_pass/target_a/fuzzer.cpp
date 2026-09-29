#include <llvm/IR/Instructions.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Support/CommandLine.h>

class Timer {
public:
  Timer(llvm::StringRef Name, bool PrintAtDestruction = false, llvm::StringRef Description = "")
  : Print{PrintAtDestruction}, Name{Name}, Description{Description} {
    Start = std::chrono::high_resolution_clock::now();;
    IsRunning = true;
  }


private:
  bool Print;
  bool IsRunning = false;
  std::string Name, Description;

  using TimePointT = 
    std::chrono::time_point<std::chrono::high_resolution_clock>;
  TimePointT Start;
  TimePointT Stop;
  
};

class ScopedPrintingTimer : private Timer {
public:
  ScopedPrintingTimer(llvm::StringRef Name, llvm::StringRef Description = "")
  : Timer{Name, true, Description} {}
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size == 0) return 0;

    llvm::StringRef Name(reinterpret_cast<const char*>(Data), std::min(Size, static_cast<size_t>(32)));
    llvm::StringRef Description(reinterpret_cast<const char*>(Data + std::min(Size, static_cast<size_t>(32))), 
                                Size > 32 ? Size - 32 : 0);  
    ScopedPrintingTimer timer(Name, Description);


    return 0;  
}