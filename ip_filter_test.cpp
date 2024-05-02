#include "util.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(PrintData, PrintData) {
    {
        util::ip_addr data = {1, 2, 3, 4};
        std::ostringstream oss;        
        util::printData(data, oss);
        ASSERT_EQ(oss.str(), "1.2.3.4");
    }
}

TEST(SplitData, SplitData) {
    {
        const auto input = util::getIpAddr("1.2.3.4");
        const util::ip_addr expected = {1, 2, 3, 4};
        ASSERT_EQ(input, expected);
    }   
}

TEST(ReverseSort, ReverseSort) {
    {
        std::vector<util::ip_addr> input = {{1, 2, 3, 4}, {2, 2, 3, 4}, {3, 2, 3, 4}};
        std::vector<util::ip_addr> expected = {{3, 2, 3, 4}, {2, 2, 3, 4}, {1, 2, 3, 4}};
        util::reverse_sort(input);
        ASSERT_EQ(expected, input);
    }
    {
        std::vector<util::ip_addr> input = {{1, 2, 3, 4}, {1, 3, 3, 4}, {3, 2, 3, 4}};
        std::vector<util::ip_addr> expected = {{3, 2, 3, 4}, {1, 3, 3, 4}, {1, 2, 3, 4}};
        util::reverse_sort(input);
        ASSERT_EQ(expected, input);
    }
}