#include "util.h"
#include <algorithm>

namespace util {

void printData(const data& d, std::ostream& oss) {
    bool isFirst = true;
    for (auto ip_part = d.cbegin(); ip_part != d.cend(); ++ip_part) {
        if (!isFirst) {
            oss << ".";
        }
        oss << *ip_part;
        isFirst = false;
    }
}

data split(const std::string &str, char d) {
    data r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos) {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

void reverse_sort(std::vector<data>& d) {
    std::sort(d.begin(), d.end(), [](const auto& lhs, const auto& rhs) -> bool {
        for (int i = 0; i < (int)lhs.size(); i++) {
            const auto numL = std::stoi(lhs.at(i));
            const auto numR = std::stoi(rhs.at(i));
            if (numL != numR) {
                return numL > numR;
            }
        }
        return false;
    });
}

} //namespace util