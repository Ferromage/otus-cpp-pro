#include "client_controller.h"
#include "tcp_client.h"
#include "version.h"
#include <iostream>
#include <fstream>
#include <boost/process/child.hpp>
#include <boost/process/io.hpp>
#include <termios.h>

namespace {
    static constexpr int TCP_PORT = 1234;

    void printHeader() {
        std::cout << "================ Messager. Version " << PROJECT_VERSION << '\n';
    }

    void clearScreen() {
        system("clear");   
    }

    std::string deleteWhitespaces(const std::string& str) {
        std::string res;
        std::transform(str.begin(), str.end(), std::back_inserter(res), [] (char c) {
            if (std::isspace(c)) {
                c = '_';
            }
            return c;
        });
        return res;
    }

    void printMessage(const std::string& user, const std::string& message, std::ostream& out) {
        out << "<" << user << ">: " << message << "\n";
    }

    std::string getStringFromInput(bool &isEscPressed) {
        constexpr int ESC = 27;

        //make terminal unbuffered
        struct termios term;
        tcgetattr(0, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(0, TCSANOW, &term);

        int c;
        std::string buf;
        while ((c = std::cin.get()) != ESC && c != '\n') {
            buf.push_back(c);
        }
        isEscPressed = c == ESC;

        term.c_lflag |= ICANON;
        tcsetattr(0, TCSANOW, &term);

        return buf;
    }
}

int main(int argc, char *argv[]) {
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
    clearScreen();
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
                    std::vector<ClientController::Message> history;
                    const auto res = clientCtrl.loadUserHistory(users[arg], history);
                    if (res.first) {
                        auto file = "/tmp/" + deleteWhitespaces(clientCtrl.localUser());
                        std::ofstream messageWindow(file);
                        boost::process::child childProc("xterm -hold -e tail -f " + file);
                        
                        for (const auto& msg : history) {
                            printMessage(msg.first, msg.second, messageWindow);
                        }
                        messageWindow.flush();

                        clientCtrl.startInteractWithUser(users[arg], [&messageWindow] (const std::string& user, const std::string& message) {
                            printMessage(user, message, messageWindow);
                            messageWindow.flush();
                        });
                                                
                        while (true) {
                            clearScreen();
                            std::cout << "You (" << clientCtrl.localUser() << ") ---> " << users[arg] << "\n" << "<your message>: ";
                            std::cout.flush();
                            
                            bool isEsc = false;
                            const auto message = getStringFromInput(isEsc);

                            if (isEsc) {
                                break;
                            }
                            clientCtrl.sendMessage(users[arg], message);
                        }
                        childProc.terminate();
                        clientCtrl.stopInteractWithUser(users[arg]);

                        clearScreen();
                        printHeader();
                        std::cout << "Hi, " << clientCtrl.localUser() << "!\n";
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