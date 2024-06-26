#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <ctime>

struct CommandBlock {
    void clear() {
        commands_.clear();
        timestamp_.reset();
    }

    std::vector<std::string> commands_;
    std::optional<std::time_t> timestamp_;
};

class Listener {
public:
    virtual ~Listener() {}
    virtual void print(const CommandBlock& block) = 0;
};

class Listenable {
public:
    virtual ~Listenable() {}
    virtual void addListener(std::weak_ptr<Listener> listener) = 0;
    virtual void removeListener(std::weak_ptr<Listener> listener) = 0;
};