#include <assert.h>
#include <cstdint>
#include <string>
#include <iostream>
#include <map>

std::string find_or(const std::map<std::string, std::string>& m, const std::string& k, const std::string v = "")
{
    auto it = m.find(k);
    return it != m.end() ? it->second : v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size)
{
  if (Size < 4) return 0;

  std::string str(reinterpret_cast<const char*>(Data),4);
  std::string val(reinterpret_cast<const char*>(Data), Size);
  std::map<std::string, std::string> m;
  m.insert(std::pair<std::string, std::string>(str, val));
  auto res = find_or(m,str);
  assert(res == val);
  return 0;
}