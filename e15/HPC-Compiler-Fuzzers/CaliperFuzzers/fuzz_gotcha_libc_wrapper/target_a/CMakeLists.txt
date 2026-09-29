add_executable(
    libcWrapperFuzzer_strNLen 
    ${CALIPER_SRC_DIR}/ext/gotcha/src/gotcha_auxv.c
    ${CALIPER_SRC_DIR}/ext/gotcha/src/libc_wrappers.c 
    fuzzer.cpp)
target_link_libraries(libcWrapperFuzzer_strNLen)