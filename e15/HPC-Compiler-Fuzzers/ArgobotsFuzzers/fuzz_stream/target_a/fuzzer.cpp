#include <stdint.h>
#include <stddef.h>

#define ABT_SUCCESS 0

typedef struct {
    int rank;
} ABTI_xstream;

ABTI_xstream global_xstream = {0};

#define ABTI_UB_ASSERT(x) if (!(x)) return -1;
#define ABTI_SETUP_LOCAL_XSTREAM(x) *x = &global_xstream

int ABT_xstream_self_rank(int *rank) {
    ABTI_UB_ASSERT(rank);

    ABTI_xstream *p_local_xstream;
    ABTI_SETUP_LOCAL_XSTREAM(&p_local_xstream);
    *rank = (int)p_local_xstream->rank;
    return ABT_SUCCESS;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < sizeof(int)) {
        return 0;
    }

    int input_rank;
    int *fuzz_rank = (int *)data;
    int result = ABT_xstream_self_rank(&input_rank);

    if (result != ABT_SUCCESS) {
        return 0;
    }

    return 0;
}