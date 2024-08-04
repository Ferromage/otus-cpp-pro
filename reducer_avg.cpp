#include <iostream>
#include <string>

int main() {
    size_t avg = 0;
    size_t count = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            avg += std::stoi(line);
            count += 1;
        } catch (...) {}
    }
    std::cout << (avg / count) << std::endl;
    return 0;
}