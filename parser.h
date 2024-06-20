#pragma once

#include <memory>

class CommandParser {
public:
    CommandParser();
    ~CommandParser();
    void parse(int blockSize);

private:
    class CommandParserImpl;
    std::unique_ptr<CommandParserImpl> impl_;
};