#include "hipSYCL/compiler/llvm-to-backend/amdgpu/LLVMToAmdgpu.hpp"
#include "hipSYCL/compiler/llvm-to-backend/AddressSpaceInferencePass.hpp"
#include "hipSYCL/compiler/llvm-to-backend/Utils.hpp"
#include "hipSYCL/compiler/sscp/IRConstantReplacer.hpp"
#include "hipSYCL/compiler/utils/LLVMUtils.hpp"
#include "hipSYCL/glue/llvm-sscp/s2_ir_constants.hpp"
#include "hipSYCL/common/filesystem.hpp"
#include "hipSYCL/common/debug.hpp"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Program.h>
#include <algorithm>
#include <memory>
#include <cassert>
#include <optional>
#include <string>
#include <system_error>
#include <vector>
#include <sstream>

/*
We can get by without a lot of headers here
*/

std::string getRocmClang(const std::string& RocmPath) {
  std::string ClangPath;

  std::string GuessedHipccPath =
      hipsycl::common::filesystem::join_path(RocmPath, std::vector<std::string>{"bin", "hipcc"});
  if (llvm::sys::fs::exists(GuessedHipccPath))
    ClangPath = GuessedHipccPath;
  else {
#if defined(ACPP_HIPCC_PATH)
    ClangPath = ACPP_HIPCC_PATH;
#else
    ClangPath = std::string("path/to/acpp/clang");
    //ClangPath = std::string("path/to/acpp/clang");
#endif
  }

  return ClangPath;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size) {
  std::string inp(reinterpret_cast<const char*>(Data),Size);
  auto res = getRocmClang(inp);
  assert(!res.empty());
  return 0;
}