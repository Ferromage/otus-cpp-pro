#include "parser.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>
#include <optional>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

class CommandParser::CommandParserImpl {
    struct CommandBlock {
        std::vector<std::string> commands;
        std::optional<std::time_t> timestamp;
    };

public:
    CommandParserImpl() {
        logThread_ = std::thread([this] {
            while (true) {
                std::unique_lock lock(logMutex_);
                logCv_.wait(lock, [this] {
                    return isExit_ || !logQueue_.empty();
                });

                while (!logQueue_.empty()) {
                    const auto& item = logQueue_.front();
                    printBuffer(item.commands, std::cout);
                    logQueue_.pop();
                }

                if (isExit_) {
                    break;
                }
            }
        });

        auto fileWorker = [this] (int index) {
            while (true) {
                std::unique_lock lock(fileMutex_);
                fileCv_.wait(lock, [this] {
                    return isExit_ || !fileQueue_.empty();
                });
        
                while (!fileQueue_.empty()) {
                    const auto& item = fileQueue_.front();

                    std::ofstream file("bulk" + std::to_string(item.timestamp.value_or(0)) + "_" + std::to_string(index) + ".log");
                    if (!file.is_open()) {
                        continue;
                    }

                    printBuffer(item.commands, file);
                    fileQueue_.pop();
                }
            
                if (isExit_) {
                    break;
                }
            }
        };

        int idx = 0;
        for (auto& thread : fileThread_) {
            thread = std::thread(fileWorker, idx++);
        }
    }

    ~CommandParserImpl() {
        isExit_ = true;

        logCv_.notify_one();
        logThread_.join();

        fileCv_.notify_all();
        for (auto& thread : fileThread_) {
            thread.join();
        }
    }

    void parse(int blockSize) {
        std::string cmd;
        int braceCnt = 0;
        CommandBlock block;
        while (std::getline(std::cin, cmd)) {
            if (cmd == "{") {
                if (braceCnt == 0) {
                    disposeCommandBlock(block);
                }
                braceCnt++;
            } else if (cmd == "}") {
                if (--braceCnt == 0) {
                    disposeCommandBlock(block);
                }
            } else {            
                if (!block.timestamp) {
                    block.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                }
                block.commands.emplace_back(std::move(cmd));

                if (braceCnt == 0 && static_cast<int>(block.commands.size()) == blockSize) {
                    disposeCommandBlock(block);
                }
            }
        }

        if (braceCnt == 0) {
            disposeCommandBlock(block);
        }
    }

private:
    void disposeCommandBlock(CommandBlock& block) {
        if (block.commands.empty()) {
            return;
        }

        {
            std::unique_lock lock(logMutex_);
            logQueue_.push(block);
            logCv_.notify_one();
        }

        {
            std::unique_lock lock(fileMutex_);
            fileQueue_.push(block);
            fileCv_.notify_all();
        }
        
        block.commands.clear();
        block.timestamp.reset();
    }

    void printBuffer(const std::vector<std::string>& commands, std::ostream& out) {
        out << "bulk: ";
        for (size_t i = 0; i < commands.size(); i++) {
            out << commands[i];

            if (i + 1 < commands.size()) {
                out << ", ";
            }
        }
        out << std::endl;    
    }

private:
    std::thread logThread_;
    std::thread fileThread_[2];
    std::queue<CommandBlock> logQueue_;
    std::queue<CommandBlock> fileQueue_;
    std::mutex logMutex_;
    std::mutex fileMutex_;
    std::condition_variable logCv_;
    std::condition_variable fileCv_;
    std::atomic<bool> isExit_ = false;
};


CommandParser::CommandParser() {
    impl_ = std::make_unique<CommandParserImpl>();
}

CommandParser::~CommandParser() {
    
}

void CommandParser::parse(int blockSize) {
    if (impl_) {
        impl_->parse(blockSize);
    }
}