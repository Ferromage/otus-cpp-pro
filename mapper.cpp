#include <iostream>
#include <string>
#include <optional>

std::optional<int> getPrice(const std::string& line) {
    static constexpr int kPricePosition = 8;

    int pos = -1;
    int commaCnt = 0;
    while ((pos = line.find(',', pos + 1)) != std::string::npos && commaCnt < kPricePosition) {
        commaCnt++;
    }

    if (commaCnt == kPricePosition) {
        const int beg = pos;
        pos = line.find(',', pos + 1);

        try {
            return std::stoi(line.substr(beg + 1, pos - beg - 1));
        } catch (...) {}
    }

    return {};
}

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        const auto price = getPrice(line);
        if (price) {
            std::cout << price.value() << std::endl;
        }
    }

    return 0;
}