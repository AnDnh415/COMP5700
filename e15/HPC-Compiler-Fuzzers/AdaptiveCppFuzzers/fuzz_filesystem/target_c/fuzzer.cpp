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

std::vector<std::string> list_regular_files(const std::string& directory, const std::string& extension) {
    auto all_files = list_regular_files(directory);
    std::vector<std::string> result;
    for (const auto& f : all_files) {
        if (fs::path(f).extension().string() == extension) {
            result.push_back(f);
        }
    }
    return result;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
    if (Size < 2) return 0;

    size_t mid = Size / 2;
    std::string directory(reinterpret_cast<const char*>(Data), mid);
    std::string extension(reinterpret_cast<const char*>(Data + mid), Size - mid);

    try {
        list_regular_files(directory, extension);
    } catch (const fs::filesystem_error&) {
    } catch (const std::invalid_argument&) {
    }

    return 0;
}