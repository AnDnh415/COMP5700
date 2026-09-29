#include <stdint.h>
#include <stddef.h>
#include <time.h>

static inline double convert_timespec_to_sec(const struct timespec *p_ts) {
    double secs;
    secs = ((double)p_ts->tv_sec) + 1.0e-9 * ((double)p_ts->tv_nsec);
    return secs;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < sizeof(struct timespec)) {
        return 0; 
    }

    struct timespec ts;
    const int64_t *input_as_int64 = (const int64_t *)data;

    ts.tv_sec = input_as_int64[0];  
    ts.tv_nsec = input_as_int64[1]; 

    (void)convert_timespec_to_sec(&ts);

    return 0;
}