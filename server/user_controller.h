#pragma once

#include "tcp_server.h"

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <tuple>
#include <optional>
#include <unordered_map>

struct UserInfo {
    UserInfo() = default;

    UserInfo(int id, const std::string& name, const std::string& password, bool isLogined, std::weak_ptr<tcpserver::Session> session) :  
        id_(id), 
        name_(name), 
        password_(password), 
        isLogined_(isLogined), 
        session_(session) {}

    std::optional<int> id_;
    std::string name_;
    std::string password_;
    bool isLogined_;
    std::weak_ptr<tcpserver::Session> session_;
};


using HistoryKey = std::pair<std::string, std::string>; //{user1, user2}

template<>
struct std::hash<HistoryKey> {
    std::size_t operator()(const HistoryKey& key) const noexcept {
        return std::hash<std::string>{}(key.first) ^ (std::hash<std::string>{}(key.second) << 1);
    }
};

class UserController {
public:
    using Message = std::pair<std::string, std::string>; //{user, message}    

    UserController() = default;
    
    std::pair<bool, std::string> registerUser(int id, const std::string& name, const std::string& password, std::weak_ptr<tcpserver::Session> session);
    std::pair<bool, std::string> loginUser(int id, const std::string& name, const std::string& password, std::weak_ptr<tcpserver::Session> session);
    std::pair<bool, std::string> listUsers(int id, std::vector<std::string>& users) const;
    std::tuple<bool, std::string, const std::vector<Message>&> loadHistory(int id, const std::string& name) const;
    void logoutUser(int id);
    std::pair<bool, std::string> sendMessage(int id, const Message& message, std::function<void(const Message& message, std::weak_ptr<tcpserver::Session>& receiverSession)> callback);

private:
    HistoryKey calculateHistoryKey(const std::string& user1, const std::string& user2) const;

    std::unordered_map<std::string, UserInfo> nameToInfoTable_;
    std::unordered_map<HistoryKey, std::vector<Message>> history_;
    mutable std::mutex mutex_;
};