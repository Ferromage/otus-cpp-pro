#include "parser.h"

#include <chrono>
#include <string>
#include <set>

class CommandParser::CommandParserImpl {
public:
    CommandParserImpl() {

    }

    void parse(int blockSize, std::istream& in) {
        std::string cmd;
        int braceCnt = 0;
        CommandBlock cmdBlock;
        while (std::getline(in, cmd)) {
            if (cmd == "{") {
                if (braceCnt == 0) {
                    disposeBuffer(cmdBlock);
                }
                braceCnt++;
            } else if (cmd == "}") {
                if (--braceCnt == 0) {
                    disposeBuffer(cmdBlock);
                }
            } else {
                if (!cmdBlock.timestamp_) {
                    cmdBlock.timestamp_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                }
                cmdBlock.commands_.emplace_back(std::move(cmd));

                if (braceCnt == 0 && static_cast<int>(cmdBlock.commands_.size()) == blockSize) {
                    disposeBuffer(cmdBlock);
                }
            }
        }
    
        if (braceCnt == 0) {
            disposeBuffer(cmdBlock);
        }  
    }    

    void addListener(std::weak_ptr<Listener> listener) {
        listeners_.insert(listener);
    }

    void removeListener(std::weak_ptr<Listener> listener) {
        if (auto it = listeners_.find(listener); it != listeners_.end()) {
            listeners_.erase(it);
        }
    }

private:
    void disposeBuffer(CommandBlock& block) {
        if (block.commands_.empty()) {
            return;
        }

        for (const auto& listener : listeners_) {
            auto lock = listener.lock();
            if (lock) {
                lock->print(block);
            }
        }
        
        block.clear();
    }

    std::set<std::weak_ptr<Listener>, std::owner_less<>> listeners_;
};

CommandParser::CommandParser() {
    impl_ = std::make_unique<CommandParserImpl>();
}

CommandParser::~CommandParser() {

}

void CommandParser::parse(int blockSize, std::istream& in) {
    if (impl_) {
        impl_->parse(blockSize, in);
    }
}

void CommandParser::addListener(std::weak_ptr<Listener> listener) {
    if (impl_) {
        impl_->addListener(listener);
    }
}
    
void CommandParser::removeListener(std::weak_ptr<Listener> listener) {
    if (impl_) {
        impl_->removeListener(listener);
    }
}