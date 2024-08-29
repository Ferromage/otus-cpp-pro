#pragma once

#include "tcp_client.h"
#include <ostream>
#include <string>
#include <vector>
#include <condition_variable>
#include <mutex>

class ClientController {
public:
    using Message = std::pair<std::string, std::string>; //{user, message}
    using MessageCallback = std::function<void(const std::string& user, const std::string& message)>;

    ClientController(std::unique_ptr<TcpClient> tcpClient);

    std::pair<bool, std::string> registerNewUser(const std::string& name, const std::string& password); //регистрация юзера
    std::pair<bool, std::string> login(const std::string& name, const std::string& password); //логин юзера
    std::pair<bool, std::string> listUsers(std::vector<std::string>& users); //возвращает список всех зареганных юзеров на сервере
    std::pair<bool, std::string> loadUserHistory(const std::string& name, std::vector<Message>& history); //возвращает историю переписки с указанным пользователем
    const std::string& localUser() const; //текущий юзер, успешно зареганный через registerNewUser или вощедший по логину через login
    const std::string& remoteUser() const; //текущий удаленный юзер
    void startInteractWithUser(const std::string& name, std::function<void(const std::string& user, const std::string& message)> callback);
    void stopInteractWithUser(const std::string& name);
    std::pair<bool, std::string> sendMessage(const std::string& name, const std::string& message);

private:
    std::string localUser_;
    std::string remoteUser_;
    std::string lastCommand_;
    std::unique_ptr<TcpClient> tcpClient_;
    std::mutex mutex_;
    std::condition_variable condVar_;
    std::string serverResponse_;
    std::string serverErrDescription_;
    std::vector<std::string> users_;
    std::vector<Message> history_;
    std::map<std::string, MessageCallback> msgCallbacks_; //user name to callback
};