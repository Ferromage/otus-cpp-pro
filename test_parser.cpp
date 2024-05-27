#include "parser.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(StaticBlock, StaticBlock) {
    {
        std::ostringstream out;
        std::istringstream in(  "cmd1\n"
                                "cmd2\n"
                                "cmd3\n"
        );

        parseCommands(3, in, out);

        ASSERT_EQ(out.str(), "bulk: cmd1, cmd2, cmd3\n");
    }
    {
        std::ostringstream out;
        std::istringstream in(  "cmd4\n"
                                "cmd5\n");

        parseCommands(3, in, out);

        ASSERT_EQ(out.str(), "bulk: cmd4, cmd5\n");
    }
}

TEST(DynamicBlock, DynamicBlock) {    
    std::ostringstream out;
    std::istringstream in(  "cmd1\n"
                            "cmd2\n"
                            "{\n"
                            "cmd3\n"
                            "cmd4\n"
                            "}\n"
                            "{\n"
                            "cmd5\n"
                            "cmd6\n"
                            "{\n"
                            "cmd7\n"
                            "cmd8\n"
                            "}\n"
                            "cmd9\n"
                            "}\n"
                            "{\n"
                            "cmd10\n"
                            "cmd11\n"
    );

    parseCommands(3, in, out);

    ASSERT_EQ(out.str(), "bulk: cmd1, cmd2\nbulk: cmd3, cmd4\nbulk: cmd5, cmd6, cmd7, cmd8, cmd9\n");    
}
