#pragma once

#include <string_view>
#include <memory>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

namespace tcpserver {

class Session;
using DataCallback = std::function<void(std::string_view data, std::string& response, std::weak_ptr<Session>, bool exit, int id)>;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, DataCallback callback, int id);
    ~Session();
    void read();
    void write(const std::string& message);

private:
    static constexpr int BUFFER_SIZE = 4096;
    tcp::socket socket_;
    char buffer_[BUFFER_SIZE];
    DataCallback callback_;
    const int id_;
};

class Server {
public:
    Server(io_context& iocontext, int port);
    void start(DataCallback callback);        

private:
    void accept(DataCallback callback);        

    tcp::acceptor acceptor_;
    int id_ = 0;
};

} //tcpserver