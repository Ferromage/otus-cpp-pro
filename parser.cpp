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
#include <map>
#include <filesystem>
#include <random>

class CommandParser::CommandParserImpl {
    struct CommandBlock {
        void clear() {
            commands.clear();
            timestamp.reset();
        }

        std::vector<std::string> commands;
        std::optional<std::time_t> timestamp;
    };

    struct Context {
        CommandBlock cmdBlock_;
        int braceCnt_ = 0;
    };

public:
    CommandParserImpl() {
        std::random_device rd;
        m_randomGen.seed(rd());

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

        auto fileWorker = [this] () {
            while (true) {
                std::unique_lock lock(fileMutex_);
                fileCv_.wait(lock, [this] {
                    return isExit_ || !fileQueue_.empty();
                });
        
                while (!fileQueue_.empty()) {
                    const auto& item = fileQueue_.front();

                    std::string fileName;
                    do {
                        const auto index = m_randomGen();
                        fileName = "bulk" + std::to_string(item.timestamp.value_or(0)) + "_" + std::to_string(index) + ".log";
                    } while (std::filesystem::exists(fileName));

                    std::ofstream file(fileName);
                    
                    if (!file.is_open()) {
                        continue;
                    }
                    
                    printBuffer(item.commands, file);
                    fileQueue_.pop();
                    file.flush();
                }
            
                if (isExit_) {
                    break;
                }
            }
        };
        
        for (auto& thread : fileThread_) {
            thread = std::thread(fileWorker);
        }
    }

    ~CommandParserImpl() {
        isExit_ = true;

        logCv_.notify_one();
        fileCv_.notify_all();
        
        logThread_.join();
        for (auto& thread : fileThread_) {
            thread.join();
        }
    }

    void parse(int id, int blockSize, std::string_view data) {
        auto& context = contexts_.try_emplace(id, Context()).first->second;
        std::string cmd;

        while (getline(data, cmd)) {
            if (cmd == "{") {
                if (context.braceCnt_ == 0) {
                    disposeCommandBlock(context.cmdBlock_);
                }
                context.braceCnt_++;
            } else if (cmd == "}") {
                if (--context.braceCnt_ == 0) {
                    disposeCommandBlock(context.cmdBlock_);
                }
            } else {            
                if (!context.cmdBlock_.timestamp) {
                    context.cmdBlock_.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                }
                context.cmdBlock_.commands.emplace_back(std::move(cmd));

                if (context.braceCnt_ == 0 && static_cast<int>(context.cmdBlock_.commands.size()) == blockSize) {
                    disposeCommandBlock(context.cmdBlock_);
                }
            }
        }
    }

    void stop(int id) {
        if (auto it = contexts_.find(id); it != contexts_.end()) {
            if (it->second.braceCnt_ == 0) {
                disposeCommandBlock(it->second.cmdBlock_);
            }
            contexts_.erase(it);
        }
    }

    void exit() {
        while (!contexts_.empty()) {
            auto it = contexts_.begin();
            stop(it->first);
        }
    }

private:
    bool getline(std::string_view& buf, std::string& cmd) {
        const auto pos = buf.find('\n');
        if (pos != std::string_view::npos) {
            cmd = std::string(buf.begin(), std::next(buf.begin(), pos));
            buf.remove_prefix(pos + 1);
            return true;
        }
        return false;
    }

    void disposeCommandBlock(CommandBlock& cmdBlock) {
        if (cmdBlock.commands.empty()) {
            return;
        }

        {
            std::unique_lock lock(logMutex_);
            logQueue_.push(cmdBlock);
            logCv_.notify_one();
        }

        {
            std::unique_lock lock(fileMutex_);
            fileQueue_.push(cmdBlock);
            fileCv_.notify_all();
        }
        
        cmdBlock.clear();
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
    std::map<int, Context> contexts_; //id to context
    std::mt19937 m_randomGen;
};


CommandParser::CommandParser() {
    impl_ = std::make_unique<CommandParserImpl>();
}

CommandParser::~CommandParser() {
    
}

void CommandParser::parse(int id, int blockSize, std::string_view data) {
    if (impl_) {
        impl_->parse(id, blockSize, data);
    }
}

void CommandParser::stop(int id) {
    if (impl_) {
        impl_->stop(id);
    }
}

void CommandParser::exit() {
    if (impl_) {
        impl_->exit();
    }
}