#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O1 -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer -I/usr/lib/x86_64-linux-gnu/openmpi/include -I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi -L/usr/lib/x86_64-linux-gnu/openmpi/lib -lmpi")
add_executable(
    configManagerFuzzer_findOr
    fuzzer.cpp
    )
target_link_libraries(configManagerFuzzer_findOr)