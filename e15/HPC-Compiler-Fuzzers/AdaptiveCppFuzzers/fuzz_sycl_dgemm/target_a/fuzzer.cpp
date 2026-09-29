#include <cstdint>
#include <cstdlib>
#include<cstring>
#include <vector>
#include<iostream>
/*
* I get this if I use the SYCL header from #include <hipSYCL/sycl/sycl.hpp>
* I get this warning:
* home/repos/HPC-Compiler-Fuzzers/build/_deps/adaptivecpp-src/include/hipSYCL/sycl/libkernel/stream.hpp:38:12: warning: format string is not a string literal (potentially insecure) [-Wformat-security] printf(s, args...),
* I should probably open an issue about this
*/
template<class T>
void _dbg(T val) { std::cout << val << std::endl; }

double error(const int Ndim, const int Mdim, double *C, double *Cgold)
{
  double err = 0.0;

  for(int i = 0; i < Ndim; ++i)
  {
    for(int j = 0; j < Mdim; ++j)
    {
      double diff = C[i * Mdim + j] - Cgold[i * Mdim + j];
      err += diff * diff;
    }
  }

  return err;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{

  if(Size < sizeof(double) * 4) {return 0;}
  constexpr size_t Mdim = 1000;
  constexpr size_t Ndim = 256;
  std::vector<double> CGold(Ndim * Mdim);
  std::vector<double> C(Ndim * Mdim);

  for(int i = 0; i < Size / sizeof(double); i++)
  {
    double val;
    std::memcpy(&val, &Data[i * sizeof(double)], sizeof(double));
    //_dbg(val);
    CGold.push_back(val);
    C.push_back(val/2.0);
  }
  //_dbg("done feeding inputs");
  double err = error(Ndim, Mdim, C.data(), CGold.data());
  return 0;
}