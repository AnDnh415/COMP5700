#include <cstdint>
#include <string>

static uint32_t gnu_hash_func(const char *str) {
  uint32_t hash = 5381;
  for (; *str != '\0'; str++) {
    hash = hash * 33 + *str;
  }
  return hash;
}
/*
* Ideal approach would be to fuzz "int prepare_symbol(struct internal_binding_t *binding)"
* in gotcha.c, the target fuzzed here is two calls away from within this func
* 
* this seems to be going on fine but non-null terminated strings break it immidiately
*/
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size)
{
    if(Size < 4) {return 0;}
    std::string str(reinterpret_cast<const char*>(Data), Size);
    gnu_hash_func(str.c_str());
    return 0;
}