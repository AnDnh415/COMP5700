#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Attributes.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>

struct OriginalParamInfo {
  std::size_t OffsetInOriginalParam;
  std::size_t OriginalParamIndex;
  llvm::SmallVector<std::string> Annotations;
};

OriginalParamInfo createFuzzedOriginalParamInfo() {
  OriginalParamInfo info;
  info.OffsetInOriginalParam = rand() % 100; 
  info.OriginalParamIndex = rand() % 10;     
  info.Annotations.push_back("annotation1");
  info.Annotations.push_back("annotation2");
  return info;
}

enum class ParamType {
  Integer,
  FoatingPoint,
  Ptr,
  OtherByVal
};

struct KernelParam {
  std::size_t ByteSize;
  std::size_t ArgByteOffset;
  std::size_t OriginalArgIndex;
  ParamType Type;
  llvm::SmallVector<std::string> Annotations;
};

struct KernelInfo {
  std::string Name;
  std::vector<KernelParam> Parameters;

  KernelInfo() = default;
  KernelInfo(const std::string &KernelName, llvm::Module &M,
             const std::vector<OriginalParamInfo>& OriginalParamInfos) {

    this->Name = KernelName;
    if(auto* F = M.getFunction(KernelName)) {

      auto* FType = F->getFunctionType();
      assert(OriginalParamInfos.size() == FType->getNumParams());

      for(int i = 0; i < FType->getNumParams(); ++i) {
        ParamType PT;

        llvm::Type* ParamT = FType->getParamType(i);
        if(ParamT->isIntegerTy()) {
          PT = ParamType::Integer;
        } else if(ParamT->isFloatingPointTy()) {
          PT = ParamType::FoatingPoint;
        } else if(ParamT->isPointerTy()) {
          if(F->hasParamAttribute(i, llvm::Attribute::ByVal)) {
            PT = ParamType::OtherByVal;
          } else {
            PT = ParamType::Ptr;
          }
        } else {
          PT = ParamType::OtherByVal;
        }

        KernelParam KP;
        
        auto BitSize = M.getDataLayout().getTypeSizeInBits(ParamT);
        assert(BitSize % CHAR_BIT == 0);
        KP.ByteSize = BitSize / CHAR_BIT;
        KP.Type = PT;
        KP.ArgByteOffset = OriginalParamInfos[i].OffsetInOriginalParam;
        KP.OriginalArgIndex = OriginalParamInfos[i].OriginalParamIndex;
        KP.Annotations = OriginalParamInfos[i].Annotations;
        this->Parameters.push_back(KP);
      }
    }
  }
};

std::unique_ptr<llvm::Module> createFuzzedModule(llvm::LLVMContext& context, const std::string& KernelName) {
    auto M = std::make_unique<llvm::Module>("fuzzed_module", context);
    llvm::FunctionType* FuncType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, KernelName, *M);


    return M; \
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size < 4) return 0; 
    std::string KernelName(reinterpret_cast<const char*>(Data), Size);

    llvm::LLVMContext context;
    auto M = createFuzzedModule(context, KernelName);

    std::vector<OriginalParamInfo> OriginalParamInfos;
    for (int i = 0; i < 3; ++i) { 
        OriginalParamInfos.push_back(createFuzzedOriginalParamInfo());
    }

    try {
        KernelInfo KI(KernelName, *M, OriginalParamInfos);
    } catch (const std::exception& e) {
        return 0;
    }

    return 0; 
}