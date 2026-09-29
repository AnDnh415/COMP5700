#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define ABT_SUCCESS 0
#define ABT_ERR_NULL -1
#define ABT_XSTREAM_NULL NULL

typedef struct ABTI_xstream {
    struct ABTI_xstream *p_prev;
    struct ABTI_xstream *p_next;
    int rank; 
} ABTI_xstream;

typedef ABTI_xstream* ABT_xstream; 

typedef struct ABTI_local {
    ABTI_xstream *p_xstream; 
} ABTI_local;

ABTI_local *lp_ABTI_local;

#define ABTI_UB_ASSERT(x) if (!(x)) return ABT_ERR_NULL;

ABTI_xstream *ABTI_xstream_get_ptr(ABT_xstream xstream) {
    return (ABTI_xstream *)xstream; 
}

#define ABTI_CHECK_NULL_XSTREAM_PTR(p) if ((p) == NULL) return ABT_ERR_NULL;

// Simulated 
ABTI_local *ABTI_local_get_local(void) {
    return lp_ABTI_local;
}

int ABTI_initialized(void) {
    return 1; // Assume initialized 
}
int ABT_xstream_get_rank(ABT_xstream xstream, int *rank) {
    ABTI_UB_ASSERT(ABTI_initialized());
    ABTI_UB_ASSERT(rank);

    ABTI_xstream *p_xstream = ABTI_xstream_get_ptr(xstream);
    ABTI_CHECK_NULL_XSTREAM_PTR(p_xstream);

    *rank = (int)p_xstream->rank;
    return ABT_SUCCESS;
}


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < sizeof(ABTI_xstream)) {
        return 0;
    }

    lp_ABTI_local = (ABTI_local *)malloc(sizeof(ABTI_local));
    if (!lp_ABTI_local) {
        return 0;
    }
    
    // Allocate 
    lp_ABTI_local->p_xstream = (ABTI_xstream *)malloc(sizeof(ABTI_xstream));
    if (!lp_ABTI_local->p_xstream) {
        free(lp_ABTI_local);
        return 0;
    }

    lp_ABTI_local->p_xstream->rank = *(int *)data; 

    ABT_xstream fuzz_xstream = lp_ABTI_local->p_xstream;

    int fuzz_rank;
    int result = ABT_xstream_get_rank(fuzz_xstream, &fuzz_rank);


    free(lp_ABTI_local->p_xstream);
    free(lp_ABTI_local);
    
    return 0;
}