cmake_minimum_required(VERSION 3.10)

# Hardcode the compilers to use (not a good practice)
#set(CMAKE_C_COMPILER /usr/bin/clang-18)
set(CMAKE_C_COMPILER /Library/Developer/CommandLineTools/usr/bin/clang)

#set(CMAKE_CXX_COMPILER /usr/bin/clang++-18)
set(CMAKE_CXX_COMPILER /Library/Developer/CommandLineTools/usr/bin/clang++)

project(CmakeTestBuild)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_PREFIX_PATH "/Library/Developer/CommandLineTools")
find_package(LLVM REQUIRED CONFIG)
#find_package(LLVM 18.1.8 REQUIRED CONFIG)
#find_package(LLVM 19.1.3 REQUIRED CONFIG)

###############################################################################
# AdaptiveCpp
FetchContent_Declare(
  AdaptiveCpp
  GIT_REPOSITORY https://github.com/AdaptiveCpp/AdaptiveCpp.git
  GIT_TAG        develop
)

# do not use FetchContent_MakeAvailable
# it will start building the fetched content
FetchContent_GetProperties(AdaptiveCpp)
if(NOT AdaptiveCpp_POPULATED)
  FetchContent_Populate(AdaptiveCpp)
endif()
message(STATUS "AdaptiveCpp source automatic src: ${AdaptiveCpp_SOURCE_DIR}")
set(ADAPTIVECPP_SRC_DIR ${CMAKE_BINARY_DIR}/_deps/adaptivecpp-src)
message(STATUS "manually set src to: ${ADAPTIVECPP_SRC_DIR}")
include_directories(${ADAPTIVECPP_SRC_DIR}/include)
#################################################################################

#################################################################################
# Caliper
FetchContent_Declare(
  Caliper
  GIT_REPOSITORY https://github.com/LLNL/Caliper.git
  GIT_TAG        master
)

FetchContent_GetProperties(Caliper)
if(NOT Caliper_POPULATED)
  FetchContent_Populate(Caliper)
endif()
message(STATUS "Caliper source automatic src: ${Caliper_SOURCE_DIR}")
set(CALIPER_SRC_DIR ${CMAKE_BINARY_DIR}/_deps/caliper-src)
message(STATUS "manually set src to: ${CALIPER_SRC_DIR}")
include_directories(${CALIPER_SRC_DIR}/include)
include_directories(${CALIPER_SRC_DIR}/src)
include_directories(${CALIPER_SRC_DIR}/ext/gotcha/include)
include_directories(${CALIPER_SRC_DIR}/ext/gotcha/src)
#configure_file(${CALIPER_SRC_DIR}/caliper-config.h.in ${CALIPER_SRC_DIR}/include/caliper/caliper-config.h)
#################################################################################

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O1 -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=fuzzer,address,undefined")
llvm_map_components_to_libnames(llvm_libs support core irreader analysis transformutils)

# add fuzzer specific CMakeLists.txt source files


# AdaptiveCpp fuzzers
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_sync_elision/target_a)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_sync_elision/target_b)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_llvm_to_spirv/target_a)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_kernel_outlining_pass/target_a)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_filesystem/target_a)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_filesystem/target_b)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_filesystem/target_c)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_filesystem/target_d)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_target_separation_pass/target_a)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_target_separation_pass/target_b)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_sycl_dgemm/target_a)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_vectorization_info/target_a)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_vectorization_info/target_b)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_introspect_struct_pass/target_a)
#add_subdirectory(./AdaptiveCppFuzzers/fuzz_introspect_struct_pass/target_b)

# Argobots fuzzers
#add_subdirectory(./ArgobotsFuzzers/fuzz_cond/target_a)
#add_subdirectory(./ArgobotsFuzzers/fuzz_stream/target_a)
#add_subdirectory(./ArgobotsFuzzers/fuzz_stream/target_b)
#add_subdirectory(./ArgobotsFuzzers/fuzz_stream/target_c)

# Caliper fuzzers
add_subdirectory(./CaliperFuzzers/fuzz_gotcha_elf_ops/target_a)
add_subdirectory(./CaliperFuzzers/fuzz_config_manager/target_a)
add_subdirectory(./CaliperFuzzers/fuzz_config_manager/target_b)
add_subdirectory(./CaliperFuzzers/fuzz_gotcha_libc_wrapper/target_a)
