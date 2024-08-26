#include "client_controller.h"
#include "tcp_client.h"
#include <iostream>

namespace {
    static constexpr int TCP_PORT = 1234;

    void printHeader() {
        std::cout << "================ Messager. Version 0.01\n";
    }
}

int main() {
    auto tcpClient = std::make_unique<TcpClient>(TCP_PORT);
    ClientController clientCtrl(std::move(tcpClient));
    int arg;

    //Page1
    printHeader();
    while (true) {
        std::cout << "Choose command: \n0 - register new user\n1 - login" << std::endl;
        std::cin >> arg;

        if (arg == 0 || arg == 1) {
            std::cout << "--- " << (arg == 0 ? "Registering a new user:" : "Login existing user:") << "\ninput name: ";
            std::cout.flush();

            std::string name;
            std::cin >> std::ws;
            std::getline(std::cin, name);
            
            std::cout << "input password: ";
            std::cout.flush();

            std::string password;
            std::cin >> std::ws;
            std::getline(std::cin, password);

            const auto res = (arg == 0) ? clientCtrl.registerNewUser(name, password) : clientCtrl.login(name, password);
            if (res.first) {
                break;
            } else {
                std::cout << "Couldn't" << (arg == 0 ? " register " : " login ") << name << " by reason: " << res.second << std::endl;
            }
        } else {
            std::cout << "Unknown command" << std::endl;
        }
    }

    //Page2
    system("clear");
    printHeader();
    std::cout << "Hi, " << clientCtrl.localUser() << "!\n";
    while (true) {
        std::cout << "Choose command: \n0 - list all users\n1 - select user" << std::endl;        
        std::cin >> arg;

        if (arg == 0) {
            std::vector<std::string> users;
            const auto res = clientCtrl.listUsers(users);
            if (res.first) {
                if (users.empty()) {
                    std::cout << "No available users\n";
                } else {
                    std::cout << "Available users:\n";
                    int i = 0;
                    for (const auto& user : users) {
                        std::cout << i++ << ": " << user << "\n";
                    }
                }
                std::cout.flush();
            } else {
                std::cout << "Couldn't list users by reason: " << res.second << std::endl;
            }
        } else if (arg == 1) {
            std::cout << "input user number: ";
            std::cout.flush();
            
            std::cin >> arg;
            std::vector<std::string> users;
            const auto res = clientCtrl.listUsers(users);
            if (res.first) {
                if (arg >= users.size()) {
                    std::cout << "wrong user number\n";
                } else {
                    //TODO: здесь надо открыть еще одно окно для переписки;
                    //в исходном окне набирать сообщения и после enter очищать экран и отправлять сообщение
                    
                    std::vector<ClientController::Message> history;
                    const auto res = clientCtrl.loadUserHistory(users[arg], history);
                    if (res.first) {
                        for (const auto& msg : history) {
                            std::cout << "<" << msg.first << ">: " << msg.second << "\n"; //TODO выводить в дочернее окно с перепиской
                        }
                        std::cout.flush();
                        clientCtrl.interactWithUser(users[arg], std::cout);
                    } else {
                        std::cout << "Couldn't load history by reason: " << res.second << std::endl;
                    }
                }   
            } else {
                std::cout << "Couldn't list users by reason: " << res.second << std::endl;
            }
        } else {
            std::cout << "Unknown command" << std::endl;
        }
    }
}