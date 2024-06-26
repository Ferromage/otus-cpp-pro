#include "parser.h"
#include "printer.h"

#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Too few arguments" << std::endl;
        return -1;
    }

    CommandParser parser;
    auto logPrinter = std::make_shared<LogPrinter>();
    auto filePrinter = std::make_shared<FilePrinter>();

    parser.addListener(logPrinter);
    parser.addListener(filePrinter);

    parser.parse(std::atoi(argv[1]), std::cin);
    
    return 0;
}