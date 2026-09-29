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

// Simulated
ABTI_local *ABTI_local_get_local(void) {
    return lp_ABTI_local;
}

static inline ABT_xstream ABTI_xstream_get_handle(ABTI_xstream *p_xstream) {
#ifndef ABT_CONFIG_DISABLE_ERROR_CHECK
    return (ABT_xstream)p_xstream; 
#else
    if (p_xstream == NULL) {
        return ABT_XSTREAM_NULL; 
    }
    return (ABT_xstream)p_xstream; 
#endif
}

// Simplified  macro
#define ABTI_SETUP_LOCAL_XSTREAM(pp_local_xstream)                             \
    do {                                                                       \
        if (pp_local_xstream) {                                                \
            *pp_local_xstream = lp_ABTI_local->p_xstream;                    \
        }                                                                      \
    } while (0)

int ABT_xstream_self(ABT_xstream *xstream) {
    ABTI_UB_ASSERT(xstream);

    ABTI_xstream *p_local_xstream;
    
    ABTI_SETUP_LOCAL_XSTREAM(&p_local_xstream);

    *xstream = ABTI_xstream_get_handle(p_local_xstream);
    return ABT_SUCCESS;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < sizeof(ABTI_xstream)) {
        return 0;
    }

    // Alloc stream
    lp_ABTI_local = (ABTI_local *)malloc(sizeof(ABTI_local));
    if (!lp_ABTI_local) {
        return 0;
    }
    
    lp_ABTI_local->p_xstream = (ABTI_xstream *)malloc(sizeof(ABTI_xstream));
    if (!lp_ABTI_local->p_xstream) {
        free(lp_ABTI_local);
        return 0;
    }

    lp_ABTI_local->p_xstream->rank = *(int *)data; 

    ABT_xstream fuzz_xstream = NULL;
    int result = ABT_xstream_self(&fuzz_xstream);

    free(lp_ABTI_local->p_xstream);
    free(lp_ABTI_local);
    
    return 0;
}
