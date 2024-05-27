#include "parser.h"

#include <chrono>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>

namespace {

struct Command {
    std::string id;
    std::time_t time;
};

void printBuffer(const std::vector<Command>& buffer, std::ostream& out) {
    out << "bulk: ";
    for (size_t i = 0; i < buffer.size(); i++) {
        out << buffer.at(i).id;

        if (i + 1 < buffer.size()) {
            out << ", ";
        }
    }
    out << std::endl;    
}

void disposeBuffer(std::vector<Command>& buffer, std::ostream& out) {
    if (buffer.empty()) {
        return;
    }

    std::ofstream file("bulk" + std::to_string(buffer.at(0).time) + ".log");
    if (!file.is_open()) {
        return;
    }

    printBuffer(buffer, out);
    printBuffer(buffer, file);
    
    buffer.clear();
}

}

void parseCommands(int blockSize, std::istream& in, std::ostream& out) {
    std::string cmd;
    int braceCnt = 0;
    std::vector<Command> cmdBuffer;
    while (std::getline(in, cmd)) {
        if (cmd == "{") {
            if (braceCnt == 0) {
                disposeBuffer(cmdBuffer, out);
            }
            braceCnt++;
        } else if (cmd == "}") {
            if (--braceCnt == 0) {
                disposeBuffer(cmdBuffer, out);
            }
        } else {
            cmdBuffer.emplace_back(std::move(cmd), std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
            if (braceCnt == 0 && static_cast<int>(cmdBuffer.size()) == blockSize) {
                disposeBuffer(cmdBuffer, out);
            }
        }
    }

    if (braceCnt == 0) {
        disposeBuffer(cmdBuffer, out);
    }  
}