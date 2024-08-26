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

    ClientController(std::unique_ptr<TcpClient> tcpClient);

    std::pair<bool, std::string> registerNewUser(const std::string& name, const std::string& password); //регистрация юзера
    std::pair<bool, std::string> login(const std::string& name, const std::string& password); //логин юзера
    std::pair<bool, std::string> listUsers(std::vector<std::string>& users); //возвращает список всех зареганных юзеров на сервере
    std::pair<bool, std::string> loadUserHistory(const std::string& name, std::vector<Message>& history); //возвращает историю переписки с указанным пользователем
    const std::string& localUser() const; //текущий юзер, успешно зареганный через registerNewUser или вощедший по логину через login
    const std::string& remoteUser() const; //текущий удаленный юзер
    void interactWithUser(const std::string& name, std::ostream& oss); //переписка с юзером через сервер

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
};