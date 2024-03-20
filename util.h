#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace util {

using data = std::vector<std::string>;

void printData(const data& d, std::ostream& oss);
data split(const std::string &str, char d);
void reverse_sort(std::vector<data>& d);

}