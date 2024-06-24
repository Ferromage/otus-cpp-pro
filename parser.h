#pragma once

#include <memory>
#include <string_view>

class CommandParser {
public:
    CommandParser();
    ~CommandParser();
    void parse(int id, int blockSize, std::string_view data);
    void stop(int id);
    void exit();

private:
    class CommandParserImpl;
    std::unique_ptr<CommandParserImpl> impl_;
};