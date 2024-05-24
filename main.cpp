#include <chrono>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>

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

void disposeBuffer(std::vector<Command>& buffer) {
    if (buffer.empty()) {
        return;
    }

    std::ofstream file("bulk" + std::to_string(buffer.at(0).time) + ".log");
    if (!file.is_open()) {
        return;
    }

    printBuffer(buffer, std::cout);
    printBuffer(buffer, file);
    
    buffer.clear();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Too few arguments" << std::endl;
        return -1;
    }

    const int blockSize = std::atoi(argv[1]);
    std::string cmd;
    int braceCnt = 0;
    std::vector<Command> cmdBuffer;
    while (std::getline(std::cin, cmd)) {
        if (cmd == "{") {
            if (braceCnt == 0) {
                disposeBuffer(cmdBuffer);
            }
            braceCnt++;
        } else if (cmd == "}") {
            if (--braceCnt == 0) {
                disposeBuffer(cmdBuffer);
            }
        } else {
            cmdBuffer.emplace_back(std::move(cmd), std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
            if (braceCnt == 0 && static_cast<int>(cmdBuffer.size()) == blockSize) {
                disposeBuffer(cmdBuffer);
            }
        }
    }

    if (braceCnt == 0) {
        disposeBuffer(cmdBuffer);
    }   
}