#include "server.h"
#include "database.h"

#include <iostream>
#include <sstream>
#include <memory>

int main(int argc, char* argv[]) {
    std::locale::global(std::locale(""));

    if (argc < 2) {
        std::cout << "Too few arguments" << std::endl;
        return 1;
    }

    const int port = atoi(argv[1]);
    if (port <= 0) {
        std::cout << "Wrong argument" << std::endl;
        return 2;
    }

    try {
        io_context io_context;
        TcpServer server(io_context, port);
        std::unique_ptr<IDatabase> database = std::make_unique<CustomDatabase>();
        
        server.start([&database] (std::string_view msg, std::string& response) {
            response.clear();

            std::istringstream iss(std::string(msg.begin(), msg.end()));
            std::string cmd;
            iss >> cmd;
            if (cmd == "INSERT") {
                std::string table, name, id;
                iss >> table >> id >> name;
                response = database->insert(table, id, name);
            } else if (cmd == "TRUNCATE") {
                std::string table;
                iss >> table;
                response = database->truncate(table);
            } else if (cmd == "INTERSECTION") {
                response = database->intersection();
            } else if (cmd == "SYMMETRIC_DIFFERENCE") {
                response = database->difference();
            } else {
                response = "unknown command\n";
            }
        });

        io_context.run();
    } catch (std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }
}