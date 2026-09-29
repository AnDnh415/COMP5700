
add_executable(SYCLDgemmFuzzer_error fuzzer.cpp)
target_link_libraries(SYCLDgemmFuzzer_error PRIVATE ${llvm_libs})