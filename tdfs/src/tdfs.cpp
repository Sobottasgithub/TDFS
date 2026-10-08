#include <iostream>

#include "utils/config.h"

int main() {
    std::cout << "TDFS!" << std::endl;

    tdfs::Config& s1 = tdfs::Config::getInstance();
    std::cout << &s1 << std::endl;

    s1.configure();
    std::cout << "R:   " << s1.getConfigReplications() << std::endl;
    std::cout << "BS:  " << s1.getConfigBlocksize() << std::endl;
    std::cout << "CND: " << s1.getConfigNameDir() << std::endl;
    std::cout << "CDD: " << s1.getConfigDataDir().at(0) << std::endl;
    std::cout << "IFN: " << s1.getConfigIsFloatingNode() << std::endl;
    std::cout << "INN: " << s1.getConfigIsNameNode() << std::endl;

    return 0;
}
