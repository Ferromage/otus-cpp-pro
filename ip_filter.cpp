#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using data = std::vector<std::string>;

void printData(const data& d) {
    bool isFirst = true;
    for (auto ip_part = d.cbegin(); ip_part != d.cend(); ++ip_part) {
        if (!isFirst) {
            std::cout << ".";
        }
        std::cout << *ip_part;
        isFirst = false;
    }
    std::cout << std::endl;
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

int main(int argc, char const *argv[]) {
    try {
        std::vector<data> ip_pool;

        for(std::string line; std::getline(std::cin, line);) {
            auto v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        //reverse lexicographically sort
        std::sort(ip_pool.begin(), ip_pool.end(), [](const auto& lhs, const auto& rhs) -> bool {
            for (int i = 0; i < 4; i++) {
                const auto numL = std::stoi(lhs.at(i));
                const auto numR = std::stoi(rhs.at(i));
                if (numL != numR) {
                    return numL > numR;
                }
            }
            return false;
        });

        for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip) {
            printData(*ip);
        }        

        //filter by first byte and output
        for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip) {
            if (ip->at(0) == "1") {
                printData(*ip);
            }
        }        

        //filter by first and second bytes and output        
        for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip) {
            if (ip->at(0) == "46" && ip->at(1) == "70") {
                printData(*ip);
            }
        }        

        //filter by any byte and output        
        for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip) {
            if (std::any_of(ip->cbegin(), ip->end(), [](const auto& octet) {return octet == "46";})) {
                printData(*ip);
            }
        }
    } catch(const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
