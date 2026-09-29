#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <cstring>

namespace fs = std::filesystem;

std::vector<std::string> list_regular_files(const std::string& directory) {
    fs::path p{directory};
    std::vector<std::string> result;
    for(const fs::directory_entry& entry : fs::directory_iterator(p)) {
        if(fs::is_regular_file(entry.status())) {
            result.push_back(entry.path().string());
        }
    }
    return result;
}
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    // Check for empty input
    if (Size == 0) return 0;

    std::string directory(reinterpret_cast<const char*>(Data), Size);

    try {
        list_regular_files(directory);
    } catch (const fs::filesystem_error& e) {
    } catch (const std::invalid_argument& e) {
    } 
    return 0;
}