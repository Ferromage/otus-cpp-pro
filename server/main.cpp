#include "tcp_server.h"

#include <string>
#include <iostream>

int main() {
    static constexpr int PORT = 1234;

    try {
        io_context io_context;
        TcpServer server(io_context, PORT);
        
        server.start([] (std::string_view msg, std::string& response, int id) {
            response = msg;
            std::cout << "RX: " << msg << std::endl;
        });

        io_context.run();
    } catch (std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }   
}