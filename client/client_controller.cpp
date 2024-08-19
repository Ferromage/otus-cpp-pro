#include "client_controller.h"

std::pair<bool, std::string> ClientController::registerNewUser(const std::string& name, const std::string& password) {
    localUser_ = name;
    return {true, ""};
}

std::pair<bool, std::string> ClientController::login(const std::string& name, const std::string& password) {
    localUser_ = name;
    return {true, ""};
}

std::vector<std::string> ClientController::listUsers() {
    return {"Vasya", "Petya", "Kolya"};
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