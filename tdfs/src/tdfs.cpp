#include <iostream>

#include "utils/config.h"

int main() {
    std::cout << "TDFS!" << std::endl;

    tdfs::Config& s1 = tdfs::Config::getInstance();
    tdfs::Config& s2 = tdfs::Config::getInstance();

    std::cout << &s1 << std::endl;
    std::cout << &s2 << std::endl;

    return 0;
}
