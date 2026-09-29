#include <assert.h>
#include <cstdint>
#include <string>
#include <iostream>
#include "caliper/common/StringConverter.h"
using namespace cali;

std::vector<std::string> to_stringlist(const std::vector<StringConverter>& list)
{
    std::vector<std::string> ret;
    ret.reserve(list.size());

    for (const StringConverter& sc : list)
        ret.push_back(sc.to_string());

    return ret;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size)
{
  if (Size < 4) return 0; 
  std::string fuzzedStr;
  const char* chPtr = reinterpret_cast<const char*>(Data);
  for(int i = 0; i < Size; i++) { fuzzedStr.push_back(chPtr[i]); }
  std::string strArr[3] = {fuzzedStr, fuzzedStr, fuzzedStr}; 
  std::vector<StringConverter> fuzzedVector;
  for(auto &str: strArr)
  {
    StringConverter sc(str);
    fuzzedVector.push_back(sc);
  }
  auto res = to_stringlist(fuzzedVector);
  assert(res.size() == 3);
  return 0;
}