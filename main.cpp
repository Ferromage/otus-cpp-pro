#include "parser.h"

#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Too few arguments" << std::endl;
        return -1;
    }

    parseCommands(std::atoi(argv[1]), std::cin, std::cout);
    return 0;
}