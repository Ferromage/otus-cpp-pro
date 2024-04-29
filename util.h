#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <array>

namespace util {

using ip_addr = std::array<int, 4>;

void printData(const ip_addr& d, std::ostream& oss);
ip_addr getIpAddr(const std::string &str);
void reverse_sort(std::vector<ip_addr>& d);

}