#include "client_controller.h"
#include "tcp_client.h"
#include <iostream>

void printHeader() {
    std::cout << "================ Messager. Version 0.01\n";
}

int main() {
    TcpClient client(1234, [] (std::string_view msg) {
        std::cout << msg << std::endl;
    });

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        client.write("Test");
    }
    return 0;




    int arg;
    ClientController clientCtrl;

    //Page1
    printHeader();
    while (true) {
        std::cout << "Choose command: \n0 - register new user\n1 - login" << std::endl;        
        std::cin >> arg;

        if (arg == 0 || arg == 1) {
            std::cout << "--- " << (arg == 0 ? "Registering a new user:" : "Login existing user:") << "\ninput name: ";
            std::cout.flush();

            std::string name;
            std::cin >> name;

            std::cout << "input password: ";
            std::cout.flush();

            std::string password;
            std::cin >> password;

            const auto res = (arg == 0) ? clientCtrl.registerNewUser(name, password) : clientCtrl.login(name, password);
            if (res.first) {
                break;
            } else {
                std::cout << "Couldn't " << (arg == 0 ? "register" : "login") << name << " by reason: " << res.second << std::endl;
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
            const auto users = clientCtrl.listUsers();
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
        } else if (arg == 1) {
            std::cout << "input user number: ";
            std::cout.flush();
            
            std::cin >> arg;
            const auto users = clientCtrl.listUsers();
            if (arg >= users.size()) {
                std::cout << "wrong user number\n";
            } else {
                const auto history = clientCtrl.loadUserHistory(users[arg]);
                for (const auto& msg : history) {
                    std::cout << msg << "\n";
                }
                std::cout.flush();
                clientCtrl.interactWithUser(users[arg], std::cout);
            }
        } else {
            std::cout << "Unknown command" << std::endl;
        }
    }
}