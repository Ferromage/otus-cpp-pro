#include "lib.h"
#include <iostream>

int main() {
    std::cout << "build " << version::get() << "\n";
    std::cout << "Hello, World!" << std::endl;
}