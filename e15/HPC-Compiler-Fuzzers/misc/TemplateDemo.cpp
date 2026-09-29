#include <iostream>
#include <string>

template<class T>
void Print(T val)
{
    std::cout << val << std::endl;
}

int main()
{
    Print<int>(5698);
    Print(1.32);
    Print<std::string>("hello");
    std::cout << "\n";

    auto a = "###";
    auto b = "X";
    auto c = 0x00;

    for (int i = 0; i < 20; i = i + 1)
    {
        for (int j = 1; j < 20; j = j + 1)
        {
               if(j % 2) {Print(b);}
               else {Print(c);}
        }   
    }
}