#include <iostream>
#include <string>
#include <sycl/sycl.hpp>

using namespace sycl;
int main()
{
    std::cout << hipsycl::sycl::detail::version_string() << std::endl;
    sycl::platform p;
    sycl::queue q;
    //std::string s(std::to_string(q.get_device()));
    std::cout << q.get_device().get_info<info::device::name>() << std::endl;
}