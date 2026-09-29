cmake_minimum_required(VERSION 3.10)
project(MyFuzzer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Set the compiler to Clang (change this to your version)
set(CMAKE_C_COMPILER /usr/bin/clang-18)
set(CMAKE_CXX_COMPILER /usr/bin/clang++-18)

# Find LLVM package (change this to your version)
find_package(LLVM 18.1.8 REQUIRED CONFIG)

# Add LLVM definitions and include directories
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Include other necessary directories (add AdaptiveCpp source directory here)
include_directories(/home/repos/AdaptiveCpp/include/)

# Set the compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O1 -fsanitize=fuzzer,address")

# Link flags for libFuzzer and Address Sanitizer
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=fuzzer,address")

add_executable(fuzzer fuzzer.cpp SimplifyKernel.cpp)

# Link against LLVM libraries
llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(fuzzer ${llvm_libs})
