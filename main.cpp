#include "server.h"
#include "async.h"

#include <string>
#include <iostream>
#include <map>

int main(int argc, char* argv[]) {
    std::locale::global(std::locale(""));

    if (argc < 3) {
        std::cout << "Too few arguments" << std::endl;
        return 1;
    }

    const int port = atoi(argv[1]);
    const int blockSize = atoi(argv[2]);
    if (port <= 0 || blockSize <= 0) {
        std::cout << "Wrong argument(s)" << std::endl;
        return 2;
    }

    try {
        std::map<int, int> clientIdToConnectId;
        io_context io_context;
        TcpServer server(io_context, port);
        
        server.start([&clientIdToConnectId, blockSize] (std::string_view msg, int id, bool stop) {
            auto p = clientIdToConnectId.try_emplace(id);
            if (p.second) {
                p.first->second = connect(blockSize);
            }
            if (stop) {
                disconnect(p.first->second);
                clientIdToConnectId.erase(p.first);
            } else {
                receive(p.first->second, msg);
            }
        });

        io_context.run();
    } catch (std::exception& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }
}