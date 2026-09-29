
add_executable(StreamFuzzer_xstreamSelf fuzzer.cpp)
target_link_libraries(StreamFuzzer_xstreamSelf PRIVATE ${llvm_libs})