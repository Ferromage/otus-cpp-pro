#include "tcp_server.h"
#include "user_controller.h"

#include <string>
#include <iostream>

namespace {
    static constexpr char kRegisterCmd[] = "<REGISTER>";
    static constexpr char kLoginCmd[] = "<LOGIN>";
    static constexpr char kListCmd[] = "<LIST>";
    static constexpr char kSuccess[] = "OK";
    static constexpr char kFail[] = "FAIL";
    static constexpr char kHistoryCmd[] = "<HISTORY>";
    static constexpr char kMessageToCmd[] = "<MESSAGE_TO>";
    static constexpr char kMessageFromCmd[] = "<MESSAGE_FROM>";

    std::string buildOkMessage(std::string_view cmd) {
        return std::string(cmd) + " " + std::string(kSuccess);
    }

    std::string buildFailMessage(std::string_view cmd, std::string_view errDescription) {
        return std::string(cmd) + " " + std::string(kFail) + " " + std::string(errDescription);
    }
}

int main() {
    static constexpr int PORT = 1234;

    try {
        io_context io_context;
        tcpserver::Server server(io_context, PORT);
        UserController userCtr;
        
        server.start([&userCtr] (std::string_view msg, std::string& response, std::weak_ptr<tcpserver::Session> session, bool exit, int id) {
            std::cout << msg << std::endl;
            
            if (exit) {
                userCtr.logoutUser(id);
            } else if (auto pos = msg.find(kRegisterCmd); pos != std::string::npos && pos == 0) {
                pos += std::size(kRegisterCmd);
                auto posEnd = msg.find(",", pos);
                if (posEnd == std::string::npos) {
                    response = buildFailMessage(kRegisterCmd, "wrong format");
                    return;
                }

                const auto name = std::string(msg.substr(pos, posEnd - pos));
                if (name.empty()) {
                    response = buildFailMessage(kRegisterCmd, "name is empty");
                    return;
                }
                
                const auto password = std::string(msg.substr(posEnd + 1));
                if (password.empty()) {
                    response = buildFailMessage(kRegisterCmd, "password is empty");
                    return;
                }

                const auto res = userCtr.registerUser(id, name, password, session);
                if (res.first) {
                    response = buildOkMessage(kRegisterCmd);
                } else {
                    response = buildFailMessage(kRegisterCmd, res.second);
                }
            } else if (pos = msg.find(kLoginCmd); pos != std::string::npos && pos == 0) {
                pos += std::size(kLoginCmd);
                auto posEnd = msg.find(",", pos);
                if (posEnd == std::string::npos) {
                    response = buildFailMessage(kLoginCmd, "wrong format");
                    return;
                }

                const auto name = std::string(msg.substr(pos, posEnd - pos));
                if (name.empty()) {
                    response = buildFailMessage(kLoginCmd, "name is empty");
                    return;
                }
                
                const auto password = std::string(msg.substr(posEnd + 1));
                if (password.empty()) {
                    response = buildFailMessage(kLoginCmd, "password is empty");
                    return;
                }
                
                const auto res = userCtr.loginUser(id, name, password, session);
                if (res.first) {
                    response = buildOkMessage(kLoginCmd);
                } else {
                    response = buildFailMessage(kLoginCmd, res.second);
                }
            } else if (pos = msg.find(kListCmd); pos != std::string::npos && pos == 0) {
                std::vector<std::string> users;
                const auto res = userCtr.listUsers(id, users);
                if (res.first) {
                    response = buildOkMessage(kListCmd) + " ";
                    for (const auto& str : users) {
                        response += str + ",";
                    }
                    response.pop_back();
                } else {
                    response = buildFailMessage(kListCmd, res.second);
                }
            } else if (pos = msg.find(kHistoryCmd); pos != std::string::npos && pos == 0) {
                pos += std::size(kHistoryCmd);
                const auto user = std::string(msg.substr(pos));

                const auto& res = userCtr.loadHistory(id, user);
                if (std::get<0>(res)) {
                    response = buildOkMessage(kHistoryCmd) + " ";
                    for (const auto& [user, msg] : std::get<2>(res)) {
                        response += "<" + user + ">:<" + msg + ">,";
                    }
                    response.pop_back();
                } else {
                    response = buildFailMessage(kHistoryCmd, std::get<1>(res));
                }
            } else if (pos = msg.find(kMessageToCmd); pos != std::string::npos && pos == 0) {
                do {
                    pos += std::size(kMessageToCmd);
                    if (msg[pos] != '<') {
                        break;
                    }

                    auto posEnd = pos + 1;
                    pos = msg.find(">:<", posEnd);
                    if (pos == std::string::npos) {
                        break;
                    }
                    
                    UserController::Message message;
                    message.first = msg.substr(posEnd, pos - posEnd);

                    posEnd = pos + 3;
                    pos = msg.find(">", posEnd);
                    if (pos == std::string::npos) {
                        break;
                    }

                    message.second = msg.substr(posEnd, pos - posEnd);

                    const auto res = userCtr.sendMessage(id, message, [] (const UserController::Message& message, std::weak_ptr<tcpserver::Session>& receiverSession) {
                        auto lock = receiverSession.lock();
                        if (lock) {
                            const auto msg = std::string(kMessageFromCmd) + " <" + message.first + ">:<" + message.second + ">";
                            lock->write(msg);
                        }
                    });
                    if (res.first) {
                        response = buildOkMessage(kMessageToCmd) + " ";
                        response += "<" + message.first + ">:<" + message.second + ">";
                    } else {
                        response = buildFailMessage(kMessageToCmd, res.second);
                    }
                    return;
                } while (false);

                response = buildFailMessage(kMessageToCmd, "wrong format");
            }
        });

        io_context.run();
    } catch (std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }   
}