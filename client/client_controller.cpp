#include "client_controller.h"
#include <iostream>

namespace {
    static constexpr char kRegisterCmd[] = "<REGISTER>";
    static constexpr char kLoginCmd[] = "<LOGIN>";
    static constexpr char kListCmd[] = "<LIST>";
    static constexpr char kSuccess[] = "OK";
    static constexpr char kFail[] = "FAIL";
    static constexpr char kServerNotAnswer[] = "server didn't respond";
    static constexpr char kTcpIsNotInit[] = "TCP client is not initialized";
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
                    }
                } else if (status == kFail) {
                    serverResponse_ = msg.substr(statusEnd + 1);
                }
            }
        }
        condVar_.notify_all();
    });
}

std::pair<bool, std::string> ClientController::registerNewUser(const std::string& name, const std::string& password) {
    if (tcpClient_) {
        std::unique_lock lock(mutex_);
        
        std::string errCode;
        serverResponse_.clear();
        lastCommand_ = kRegisterCmd;
        if (tcpClient_->write(lastCommand_ + " " + name + "," + password, errCode)) {
            const auto condRes = condVar_.wait_for(lock, std::chrono::seconds(3), [this] {return !serverResponse_.empty();});
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
    if (tcpClient_) {
        std::unique_lock lock(mutex_);
        
        std::string errCode;
        serverResponse_.clear();
        lastCommand_ = kLoginCmd;
        if (tcpClient_->write(lastCommand_ + " " + name + "," + password, errCode)) {
            const auto condRes = condVar_.wait_for(lock, std::chrono::seconds(3), [this] {return !serverResponse_.empty();});
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
            const auto condRes = condVar_.wait_for(lock, std::chrono::seconds(3), [this] {return !serverResponse_.empty();});
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

const std::string& ClientController::localUser() {
    return localUser_;
}

const std::string& ClientController::remoteUser() {
    return remoteUser_;
}

std::vector<std::string> ClientController::loadUserHistory(const std::string& name) {
    remoteUser_ = name;
    return {};
}

void ClientController::interactWithUser(const std::string& name, std::ostream& oss) {

}