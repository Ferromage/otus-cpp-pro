#include "printer.h"
#include <iostream>
#include <fstream>

namespace {

void printBuffer(const CommandBlock& block, std::ostream& out) {
    out << "bulk: ";
    for (size_t i = 0; i < block.commands_.size(); i++) {
        out << block.commands_[i];

        if (i + 1 < block.commands_.size()) {
            out << ", ";
        }
    }
    out << std::endl;    
}

}

void LogPrinter::print(const CommandBlock& block) {
    printBuffer(block, std::cout);
}

void FilePrinter::print(const CommandBlock& block) {
    std::ofstream file("bulk" + std::to_string(block.timestamp_.value_or(0)) + ".log");
    if (!file.is_open()) {
        return;
    }
    printBuffer(block, file);
}
