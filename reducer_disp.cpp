#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Too few arguments\n";
        return -1;
    }

    long long avg = 0;
    {
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Cannot open file " << argv[1] << "\n";
            return -2;
        }

        std::string avgStr;
        file >> avgStr;

        try {
            avg = std::stoll(avgStr);
        } catch (...) {
            std::cerr << "Invalid file content\n";
            return -3;
        }
    }

    if (avg <= 0) {
        std::cerr << "Average value cannot be negative or null\n";
        return -4;
    }

    size_t count = 0;
    size_t sum = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            const auto price = std::stoll(line);
            sum += (price - avg) * (price - avg);
            count += 1;
        } catch (...) {}
    }
    std::cout << (sum / count) << std::endl;
    return 0;
}