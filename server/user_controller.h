#pragma once

#include "tcp_server.h"

#include <string>
#include <memory>
#include <vector>
#include <mutex>
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

class UserController {
public:
    UserController() = default;

    std::pair<bool, std::string> registerUser(int id, const std::string& name, const std::string& password, std::weak_ptr<tcpserver::Session> session);
    std::pair<bool, std::string> loginUser(int id, const std::string& name, const std::string& password, std::weak_ptr<tcpserver::Session> session);
    std::pair<bool, std::string> listUsers(int id, std::vector<std::string>& users) const;
    void logoutUser(int id);

private:
    std::unordered_map<std::string, UserInfo> nameToInfoTable_;
    mutable std::mutex mutex_;
};