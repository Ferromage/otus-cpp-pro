#include "util.h"
#include <algorithm>

int main(int, char const**) {
    using namespace util;

    try {
        std::vector<data> ip_pool;

        for(std::string line; std::getline(std::cin, line);) {
            auto v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        //reverse lexicographically sort
        reverse_sort(ip_pool);
        for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip) {
            printData(*ip, std::cout);
            std::cout << std::endl;
        }        

        //filter by first byte and output
        for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip) {
            if (ip->at(0) == "1") {
                printData(*ip, std::cout);
                std::cout << std::endl;
            }
        }        

        //filter by first and second bytes and output        
        for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip) {
            if (ip->at(0) == "46" && ip->at(1) == "70") {
                printData(*ip, std::cout);
                std::cout << std::endl;
            }
        }        

        //filter by any byte and output        
        for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip) {
            if (std::any_of(ip->cbegin(), ip->end(), [](const auto& octet) {return octet == "46";})) {
                printData(*ip, std::cout);
                std::cout << std::endl;
            }
        }
    } catch(const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
