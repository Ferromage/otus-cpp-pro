#include "async.h"
#include "parser.h"

#include <map>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>

namespace {
    int contextCounter = 0;

    class Worker {
        struct QueueItem {
            int id;
            int blockSize;
            std::string_view cmdBuf;
            bool isStop = false;
        };

    public:
        Worker() {
            thread_ = std::thread([this] {
                parser_ = std::make_unique<CommandParser>();

                while (true) {
                    std::unique_lock lock(m_);
                    cv_.wait(lock, [this] {
                        return isExit_ || !queue_.empty();
                    });

                    while (!queue_.empty()) {
                        const auto& item = queue_.front();
                        if (parser_) {
                            if (item.isStop) {
                                parser_->stop(item.id);
                            } else {
                                parser_->parse(item.id, item.blockSize, item.cmdBuf);
                            }
                        }
                        queue_.pop();
                    }

                    if (isExit_) {
                        if (parser_) {
                            parser_->exit();
                        }
                        break;
                    }
                }
            });
        }

        ~Worker() {
            isExit_ = true;
            cv_.notify_one();
            thread_.join();
        }

        void addTask(int id, int blockSize, std::string_view cmdBuf) {
            {
                std::lock_guard lock(m_);
                queue_.push({id, blockSize, cmdBuf, false});
            }
            cv_.notify_one();
        }

        void removeTask(int id) {
            {
                std::unique_lock lock(m_);
                queue_.push({id, 0, {}, true});
            }
            cv_.notify_one();
        }
        
    private:
        std::unique_ptr<CommandParser> parser_;
        std::queue<QueueItem> queue_;
        std::mutex m_;
        std::condition_variable cv_;
        std::thread thread_;
        std::atomic<bool> isExit_ = false;
    };


    std::map<int, int> idToBlockSize;
    std::mutex mutex;
    Worker worker;
}


int connect(int blockSize) {
    std::lock_guard lock(mutex);

    idToBlockSize[contextCounter] = blockSize;
    return contextCounter++;
}


void receive(int id, std::string_view cmdBuf) {
    int blockSize = 0;    
    {
        std::lock_guard lock(mutex);
        if (auto it = idToBlockSize.find(id); it != idToBlockSize.end()) {
            blockSize = it->second;
        }
    }

    if (blockSize) {
        worker.addTask(id, blockSize, cmdBuf);
    }
}


void disconnect(int id) {
    bool isItemFound = false;
    {
        std::lock_guard lock(mutex);
        isItemFound = idToBlockSize.count(id);
    }
    
    if (isItemFound) {
        worker.removeTask(id);

        std::lock_guard lock(mutex);
        idToBlockSize.erase(idToBlockSize.find(id));
    }
}