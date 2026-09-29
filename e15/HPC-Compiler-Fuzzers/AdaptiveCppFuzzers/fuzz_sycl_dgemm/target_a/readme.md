This target is not bieng fuzzed effectively. The target `double error(const int Ndim, const int Mdim, double *C, double *Cgold)` is used to calculate approximate error for each element in a 2D `cl::sycl::buffer` object and to use anything from the `cl::sycl` namespace, we need a compiler that provides SYCL implementation. There are two solutions:

**Option 1:** install AdaptiveCpp compiler in the CI runner and then use it to create the fuzzer. This can work as I have tested it locally.

**Option 2:** install DPC++ compiler in the CI runner and then use it to create the fuzzer.