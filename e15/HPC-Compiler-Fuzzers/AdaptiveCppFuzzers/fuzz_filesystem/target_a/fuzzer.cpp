#include <string>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

std::string join_path(const std::string& base, const std::string& extra) {
  return (fs::path(base) / extra).string();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Size < 4) return 0;

  uint16_t base_len = Data[0] + (Data[1] << 8);  
  uint16_t extra_len = Data[2] + (Data[3] << 8); 

  base_len = std::min<size_t>(base_len, Size - 4);
  extra_len = std::min<size_t>(extra_len, Size - 4 - base_len);

  std::string base(reinterpret_cast<const char*>(Data + 4), base_len);
  std::string extra(reinterpret_cast<const char*>(Data + 4 + base_len), extra_len);

  std::string result = join_path(base, extra);

  return 0;
}