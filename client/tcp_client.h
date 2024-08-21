#pragma once

#include <boost/asio.hpp>
#include <thread>
#include <atomic>

using namespace boost::asio;
using boost::asio::ip::tcp;

class TcpClient {
public:
    using DataCallback = std::function<void(std::string_view data)>;

    TcpClient(int port, DataCallback callback = nullptr);
    ~TcpClient();

    void write(const std::string& message);

private:
    void read();

    DataCallback callback_;
    boost::asio::io_context context_;
    tcp::socket socket_;
    tcp::resolver resolver_;
    std::thread thread_;
    bool stop_;
    char buffer_[8192];
    std::atomic_bool isConnected_;
};