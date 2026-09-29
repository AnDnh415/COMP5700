#include <cstdint>
#include <iostream>

size_t gotcha_strnlen(const char *s, size_t max_length) {
  size_t i;
  for (i = 0; s[i] && i < max_length; i++)
    ;
  return i;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size)
{
    if(Size < 4) { return 0; }
    auto res = gotcha_strnlen(reinterpret_cast<const char*>(Data), Size - 1); //removing -1 will make s[i] throw errors, which makes sense
    return 0;
}