#pragma once

#include <string_view>
#include <memory>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

class TcpServer {
public:
    using DataCallback = std::function<void(std::string_view data, std::string& response)>;

    TcpServer(io_context& iocontext, int port);
    void start(DataCallback callback);        

private:
    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket, DataCallback callback);
        void read();

    private:
        static constexpr int BUFFER_SIZE = 256;
        tcp::socket socket_;
        char buffer_[BUFFER_SIZE];
        DataCallback callback_;
    };

    void accept(DataCallback callback);

    tcp::acceptor acceptor_;
};