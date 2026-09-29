#include <string>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <random>

namespace fs = std::filesystem;

// Function to generate a random number (utility function to emulate `random_number` in the original code)
template<class T>
inline T random_number() {
  thread_local std::random_device rd;
  thread_local std::mt19937 gen{rd()};
  thread_local std::uniform_int_distribution<T> distribution{0};

  return distribution(gen);
}

bool atomic_write(const std::string &filename, const std::string &data) {
    fs::path p{filename};

    std::string temp_file = std::to_string(random_number<std::size_t>()) + ".tmp";
    fs::path tmp_path = p.parent_path() / temp_file;

    std::ofstream ostr{tmp_path, std::ios::binary | std::ios::out | std::ios::trunc};
    if (!ostr.is_open()) return false;

    ostr.write(data.data(), data.size());
    ostr.close();

    fs::rename(tmp_path, p);

    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size < 2) return 0; 
    size_t mid = Size / 2;
    std::string filename(reinterpret_cast<const char*>(Data), mid);
    std::string file_data(reinterpret_cast<const char*>(Data + mid), Size - mid);
    
    atomic_write(filename, file_data);

    return 0;
}