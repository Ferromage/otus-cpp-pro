#include "util.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(PrintData, PrintData) {
    {
        util::data data = {};
        std::ostringstream oss;
        util::printData(data, oss);
        ASSERT_TRUE(oss.str().empty());
    }
    {
        util::data data = {"1"};
        std::ostringstream oss;        
        util::printData(data, oss);
        ASSERT_EQ(oss.str(), "1");
    }
    {
        util::data data = {"1", "2"};
        std::ostringstream oss;        
        util::printData(data, oss);
        ASSERT_EQ(oss.str(), "1.2");
    }
    {
        util::data data = {"1", "2", "3"};
        std::ostringstream oss;        
        util::printData(data, oss);
        ASSERT_EQ(oss.str(), "1.2.3");
    }
    {
        util::data data = {"1", "2", "3", "4"};
        std::ostringstream oss;        
        util::printData(data, oss);
        ASSERT_EQ(oss.str(), "1.2.3.4");
    }
    {
        util::data data = {"1", "2", "3", "4", "5"};
        std::ostringstream oss;        
        util::printData(data, oss);
        ASSERT_EQ(oss.str(), "1.2.3.4.5");
    }
}

TEST(SplitData, SplitData) {
    {
        const auto input = util::split("", '.');
        const util::data expected = {""};
        ASSERT_EQ(input, expected);
    }
    {
        const auto input = util::split("11", '.');
        const util::data expected = {"11"};
        ASSERT_EQ(input, expected);
    }
    {
        const auto input = util::split("..", '.');
        const util::data expected = {"", "", ""};
        ASSERT_EQ(input, expected);
    }
    {
        const auto input = util::split("11.", '.');
        const util::data expected = {"11", ""};
        ASSERT_EQ(input, expected);
    }
    {
        const auto input = util::split(".11", '.');
        const util::data expected = {"", "11"};
        ASSERT_EQ(input, expected);
    }
    {
        const auto input = util::split("11.22", '.');
        const util::data expected = {"11", "22"};
        ASSERT_EQ(input, expected);
    }
    {
        const auto input = util::split("1 2 3 4", ' ');
        const util::data expected = {"1", "2", "3", "4"};
        ASSERT_EQ(input, expected);
    }
    {
        const auto input = util::split("1\t2\t3\t4", '\t');
        const util::data expected = {"1", "2", "3", "4"} ;
        ASSERT_EQ(input, expected);
    }
    {
        const auto input = util::split("1.2.3.4", '.');
        const util::data expected = {"1", "2", "3", "4"};
        ASSERT_EQ(input, expected);
    }   
}

TEST(ReverseSort, ReverseSort) {
    {
        std::vector<util::data> input = {{"1"}, {"2"}, {"3"}};
        std::vector<util::data> expected = {{"3"}, {"2"}, {"1"}};
        util::reverse_sort(input);
        ASSERT_EQ(expected, input);
    }
    {
        std::vector<util::data> input = {{"3"}, {"2"}, {"1"}};
        std::vector<util::data> expected = {{"3"}, {"2"}, {"1"}};
        util::reverse_sort(input);
        ASSERT_EQ(expected, input);
    }
    {
        std::vector<util::data> input = {{"1", "2", "3", "4"}, {"2", "2", "3", "4"}, {"3", "2", "3", "4"}};
        std::vector<util::data> expected = {{"3", "2", "3", "4"}, {"2", "2", "3", "4"}, {"1", "2", "3", "4"}};
        util::reverse_sort(input);
        ASSERT_EQ(expected, input);
    }
    {
        std::vector<util::data> input = {{"1", "2", "3", "4"}, {"1", "3", "3", "4"}, {"3", "2", "3", "4"}};
        std::vector<util::data> expected = {{"3", "2", "3", "4"}, {"1", "3", "3", "4"}, {"1", "2", "3", "4"}};
        util::reverse_sort(input);
        ASSERT_EQ(expected, input);
    }
}