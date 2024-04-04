#include "util.h"
#include <algorithm>

namespace util {

void printData(const ip_addr& ip, std::ostream& oss) {
    bool isFirst = true;
    for (auto octet : ip) {
        if (!isFirst) {
            oss << ".";
        }
        oss << octet;
        isFirst = false;
    }
}

ip_addr getIpAddr(const std::string &str) {
    ip_addr r;

    try {
        std::string::size_type start = 0;
        std::string::size_type stop = str.find_first_of('.');
        int i = 0;
        while (stop != std::string::npos) {
            r[i++] = std::stoi(str.substr(start, stop - start));

            start = stop + 1;
            stop = str.find_first_of('.', start);
        }

        r[i] = std::stoi(str.substr(start));
    } catch (...) {
        r.fill(0);
    }

    return r;
}

void reverse_sort(std::vector<ip_addr>& d) {
    std::sort(d.begin(), d.end(), [](const auto& lhs, const auto& rhs) -> bool {
        for (int i = 0; i < (int)lhs.size(); i++) {
            const auto numL = lhs.at(i);
            const auto numR = rhs.at(i);
            if (numL != numR) {
                return numL > numR;
            }
        }
        return false;
    });
}

} //namespace util