#pragma once

#include <ostream>
#include <string>
#include <vector>

class ClientController {
public:
    ClientController() = default;

    std::pair<bool, std::string> registerNewUser(const std::string& name, const std::string& password); //регистрация юзера
    std::pair<bool, std::string> login(const std::string& name, const std::string& password); //логин юзера
    std::vector<std::string> listUsers(); //возвращает список всех зареганных юзеров на сервере
    std::vector<std::string> loadUserHistory(const std::string& name); //возвращает историю переписки с указанным пользователем
    const std::string& localUser(); //текущий юзер, успешно зареганный через registerNewUser или вощедший по логину через login
    const std::string& remoteUser(); //текущий удаленный юзер
    void interactWithUser(const std::string& name, std::ostream& oss); //переписка с юзером через сервер

private:
    std::string localUser_;
    std::string remoteUser_;
};