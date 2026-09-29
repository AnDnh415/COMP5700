#include <cstdint>
#include <string>

static uint32_t gnu_hash_func(const char *str) {
  uint32_t hash = 5381;
  for (; *str != '\0'; str++) {
    hash = hash * 33 + *str;
  }
  return hash;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size)
{
    if(Size < 4) {return 0;}
    const char *str = reinterpret_cast<const char*>(Data);
    gnu_hash_func(str);
    return 0;
}