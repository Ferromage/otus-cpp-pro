#include "user_controller.h"
#include <algorithm>
#include <iostream>

std::pair<bool, std::string> UserController::registerUser(int id, const std::string& name, const std::string& password, std::weak_ptr<tcpserver::Session> session) {
    std::lock_guard lock(mutex_);

    auto res = nameToInfoTable_.try_emplace(name, id, name, password, true, session);
    if (!res.second) {
        return {false, "User with such name is already registered"};
    }

    return {true, ""};
}

std::pair<bool, std::string> UserController::loginUser(int id, const std::string& name, const std::string& password, std::weak_ptr<tcpserver::Session> session) {
    std::lock_guard lock(mutex_);
    
    if (auto it = nameToInfoTable_.find(name); it != nameToInfoTable_.end()) {
        if (it->second.isLogined_) {
            return {false, "This user is already logined on another device"};
        }
        if (password != it->second.password_) {
            return {false, "Wrong password"};
        }

        it->second.isLogined_ = true;
        it->second.id_ = id;
        it->second.session_ = session;
        
        return {true, ""};
    }

    return {false, "No such user"};
}

void UserController::logoutUser(int id) {
    std::lock_guard lock(mutex_);

    auto it = nameToInfoTable_.begin();
    while (it != nameToInfoTable_.end()) {
        if (it->second.id_.has_value() && it->second.id_ == id) {
            break;
        }
        ++it;
    }

    if (it != nameToInfoTable_.end()) {
        it->second.isLogined_ = false;
        it->second.id_.reset();
        it->second.session_.reset();
    }
}

std::pair<bool, std::string> UserController::listUsers(int id, std::vector<std::string>& users) const {
    std::lock_guard lock(mutex_);

    users.clear();
    auto it = nameToInfoTable_.begin();
    while (it != nameToInfoTable_.end()) {
        if (it->second.id_.has_value() && it->second.id_ == id) {
            break;
        }
        ++it;
    }

    if (it == nameToInfoTable_.end()) {
        return {false, "User is not logined or registered"};
    }
    
    for (const auto& [name, info] : nameToInfoTable_) {
        if (name != it->first) {    //exclude current user
            users.emplace_back(name);
        }
    }

    return {true, ""};
}