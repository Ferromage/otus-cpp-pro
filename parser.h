#pragma once

#include "common.h"
#include <iostream>
#include <memory>

class CommandParser : public Listenable {
public:
    CommandParser();
    ~CommandParser();
    void parse(int blockSize, std::istream& in);
    void addListener(std::weak_ptr<Listener> listener) override;
    void removeListener(std::weak_ptr<Listener> listener) override;

private:
    class CommandParserImpl;
    std::unique_ptr<CommandParserImpl> impl_;
};