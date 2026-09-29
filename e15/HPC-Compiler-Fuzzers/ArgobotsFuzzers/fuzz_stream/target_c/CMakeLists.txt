
add_executable(StreamFuzzer_xstreamGetRank fuzzer.cpp)
target_link_libraries(StreamFuzzer_xstreamGetRank PRIVATE ${llvm_libs})