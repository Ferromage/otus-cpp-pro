#include "client_controller.h"
#include <iostream>
#include <algorithm>

namespace {
    static constexpr int kTimeoutInSeconds = 3;

    static constexpr char kRegisterCmd[] = "<REGISTER>";
    static constexpr char kLoginCmd[] = "<LOGIN>";
    static constexpr char kListCmd[] = "<LIST>";
    static constexpr char kHistoryCmd[] = "<HISTORY>";
    static constexpr char kMessageToCmd[] = "<MESSAGE_TO>";
    static constexpr char kMessageFromCmd[] = "<MESSAGE_FROM>";
    static constexpr char kSuccess[] = "OK";
    static constexpr char kFail[] = "FAIL";
    static constexpr char kServerNotAnswer[] = "server didn't respond";
    static constexpr char kTcpIsNotInit[] = "TCP client is not initialized";
    static constexpr char kUsernameIsInvalid[] = "Username must not contain symbols '<', '>', ',', ':'";
    static constexpr char kPasswordIsInvalid[] = "Password must not contain symbols '<', '>', ',', ':'";
    static constexpr char kMessageIsEmpty[] = "Message is empty";

    bool isUserStringValid(const std::string& str) {
        return std::find_if(str.begin(), str.end(), [] (char sym) {
            return sym == '<' || sym == '>' || sym == ',' || sym == ':';
        }) == str.end();
    }
}

ClientController::ClientController(std::unique_ptr<TcpClient> tcpClient) : tcpClient_(std::move(tcpClient)) {
    tcpClient_->start([this] (std::string_view msg) {
        {
            std::unique_lock lock(mutex_);

            auto pos = msg.find(lastCommand_);
            if (pos != std::string::npos && pos == 0) {
                pos += lastCommand_.size() + 1;
                
                auto statusEnd = msg.find(" ", pos);
                const auto status = (statusEnd != std::string::npos) ? msg.substr(pos, statusEnd - pos) : msg.substr(pos);
                
                if (status == kSuccess) {
                    serverResponse_ = kSuccess;

                    if (lastCommand_ == kListCmd) {
                        users_.clear();
                        
                        if (statusEnd != std::string::npos) {
                            statusEnd += 1;
                            do {
                                pos = msg.find(",", statusEnd);
                                if (pos != std::string::npos) {
                                    users_.emplace_back(msg.substr(statusEnd, pos - statusEnd));
                                } else {
                                    users_.emplace_back(msg.substr(statusEnd));
                                }
                                statusEnd = pos + 1;
                            } while (pos != std::string::npos);
                        }
                    } else if (lastCommand_ == kMessageToCmd) {
                        Message message;
                        
                        if (statusEnd != std::string::npos && msg[statusEnd + 1] == '<') {
                            statusEnd += 2;
                            pos = msg.find(">:<", statusEnd);
                            if (pos != std::string::npos) {
                                message.first = msg.substr(statusEnd, pos - statusEnd);
                                statusEnd = pos + 3;
                                pos = msg.find(">", statusEnd);
                                if (pos != std::string::npos) {
                                    message.second = msg.substr(statusEnd, pos - statusEnd);
                                }

                                if (auto it = msgCallbacks_.find(message.first); it != msgCallbacks_.end()) {
                                    it->second(localUser_, message.second);
                                }
                            }
                        }
                    } else if (lastCommand_ == kHistoryCmd) {
                        history_.clear();

                        if (statusEnd != std::string::npos && msg[statusEnd + 1] == '<') {
                            statusEnd += 2;
                            while (true) {
                                std::pair<std::string, std::string> item;
                                
                                pos = msg.find(">:<", statusEnd);
                                if (pos != std::string::npos) {
                                    item.first = msg.substr(statusEnd, pos - statusEnd);
                                } else {
                                    break;
                                }
                                statusEnd = pos + 3;

                                pos = msg.find(">", statusEnd);
                                if (pos != std::string::npos) {
                                    item.second = msg.substr(statusEnd, pos - statusEnd);
                                } else {
                                    break;
                                }

                                history_.emplace_back(item);

                                pos = msg.find(">,<", pos);
                                if (pos != std::string::npos) {
                                    statusEnd = pos + 3;
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                } else if (status == kFail) {
                    serverResponse_ = msg.substr(statusEnd + 1);
                }
            } else if (pos = msg.find(kMessageFromCmd); pos != std::string::npos && pos == 0) {
                pos += std::size(kMessageFromCmd);
                if (msg[pos] == '<') {
                    Message message;
                    pos += 1;
                    auto statusEnd = pos;
                    pos = msg.find(">:<", statusEnd);
                    if (pos != std::string::npos) {
                        message.first = msg.substr(statusEnd, pos - statusEnd);
                        statusEnd = pos + 3;
                        pos = msg.find(">", statusEnd);
                        if (pos != std::string::npos) {
                            message.second = msg.substr(statusEnd, pos - statusEnd);
                        }

                        if (auto it = msgCallbacks_.find(message.first); it != msgCallbacks_.end()) {
                            it->second(message.first, message.second);
                        }
                    }
                }
            }
        }
        condVar_.notify_all();
    });
}

std::pair<bool, std::string> ClientController::registerNewUser(const std::string& name, const std::string& password) {
    if (!isUserStringValid(name)) {
        return {false, kUsernameIsInvalid};
    }
    if (!isUserStringValid(password)) {
        return {false, kPasswordIsInvalid};
    }

    if (tcpClient_) {
        std::unique_lock lock(mutex_);
        
        std::string errCode;
        serverResponse_.clear();
        lastCommand_ = kRegisterCmd;
        if (tcpClient_->write(lastCommand_ + " " + name + "," + password, errCode)) {
            const auto condRes = condVar_.wait_for(lock, std::chrono::seconds(kTimeoutInSeconds), [this] {return !serverResponse_.empty();});
            if (!condRes) {
                return {false, kServerNotAnswer};
            }
            if (serverResponse_ != kSuccess) {
                return {false, serverResponse_};
            }
            localUser_ = name;
            return {true, ""};
        } else {
            return {false, errCode};
        }
    } else {
        return {false, kTcpIsNotInit};
    }
}

std::pair<bool, std::string> ClientController::login(const std::string& name, const std::string& password) {
    if (!isUserStringValid(name)) {
        return {false, kUsernameIsInvalid};
    }
    if (!isUserStringValid(password)) {
        return {false, kPasswordIsInvalid};
    }

    if (tcpClient_) {
        std::unique_lock lock(mutex_);
        
        std::string errCode;
        serverResponse_.clear();
        lastCommand_ = kLoginCmd;
        if (tcpClient_->write(lastCommand_ + " " + name + "," + password, errCode)) {
            const auto condRes = condVar_.wait_for(lock, std::chrono::seconds(kTimeoutInSeconds), [this] {return !serverResponse_.empty();});
            if (!condRes) {
                return {false, kServerNotAnswer};
            }
            if (serverResponse_ != kSuccess) {
                return {false, serverResponse_};
            }
            localUser_ = name;
            return {true, ""};
        } else {
            return {false, errCode};
        }
    } else {
        return {false, kTcpIsNotInit};
    }
}

std::pair<bool, std::string> ClientController::listUsers(std::vector<std::string>& users) {
    if (tcpClient_) {
        std::unique_lock lock(mutex_);
        
        std::string errCode;
        serverResponse_.clear();
        lastCommand_ = kListCmd;
        if (tcpClient_->write(lastCommand_, errCode)) {
            const auto condRes = condVar_.wait_for(lock, std::chrono::seconds(kTimeoutInSeconds), [this] {return !serverResponse_.empty();});
            if (!condRes) {
                return {false, kServerNotAnswer};
            }
            if (serverResponse_ != kSuccess) {
                return {false, serverResponse_};
            }
            users = std::move(users_);
            return {true, ""};
        } else {
            return {false, errCode};
        }
    } else {
        return {false, kTcpIsNotInit};
    }
}

const std::string& ClientController::localUser() const {
    return localUser_;
}

const std::string& ClientController::remoteUser() const {
    return remoteUser_;
}

std::pair<bool, std::string> ClientController::loadUserHistory(const std::string& name, std::vector<Message>& history) {
    if (tcpClient_) {
        std::unique_lock lock(mutex_);
        
        std::string errCode;
        serverResponse_.clear();
        lastCommand_ = kHistoryCmd;
        if (tcpClient_->write(lastCommand_ + " " + name, errCode)) {
            const auto condRes = condVar_.wait_for(lock, std::chrono::seconds(kTimeoutInSeconds), [this] {return !serverResponse_.empty();});
            if (!condRes) {
                return {false, kServerNotAnswer};
            }
            if (serverResponse_ != kSuccess) {
                return {false, serverResponse_};
            }
            
            remoteUser_ = name;
            history = std::move(history_);
            return {true, ""};
        } else {
            return {false, errCode};
        }
    } else {
        return {false, kTcpIsNotInit};
    }
}

void ClientController::startInteractWithUser(const std::string& name, std::function<void(const std::string& user, const std::string& message)> callback) {
    std::unique_lock lock(mutex_);
    msgCallbacks_[name] = callback;
}

void ClientController::stopInteractWithUser(const std::string& name) {
    std::unique_lock lock(mutex_);
    if (auto it = msgCallbacks_.find(name); it != msgCallbacks_.end()) {
        msgCallbacks_.erase(it);
    }
}

std::pair<bool, std::string> ClientController::sendMessage(const std::string& name, const std::string& message) {
    if (message.empty()) {
        return {false, kMessageIsEmpty};
    }

    if (tcpClient_) {
        std::unique_lock lock(mutex_);
        
        std::string errCode;
        serverResponse_.clear();
        lastCommand_ = kMessageToCmd;
        if (tcpClient_->write(lastCommand_ + " <" + name + ">:<" + message + ">", errCode)) {
            const auto condRes = condVar_.wait_for(lock, std::chrono::seconds(kTimeoutInSeconds), [this] {return !serverResponse_.empty();});
            if (!condRes) {
                return {false, kServerNotAnswer};
            }
            if (serverResponse_ != kSuccess) {
                return {false, serverResponse_};
            }
            return {true, ""};
        } else {
            return {false, errCode};
        }
    } else {
        return {false, kTcpIsNotInit};
    }
}
