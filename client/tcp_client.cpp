#include "tcp_client.h"
#include <iostream>
#include <chrono>

TcpClient::TcpClient(int port, DataCallback callback) : callback_(callback), socket_(context_), resolver_(context_), stop_{false}, isConnected_{false} {
    thread_ = std::thread([this, port] {
        boost::asio::ip::address addr;
        addr.from_string("127.0.0.1");
        boost::asio::ip::tcp::endpoint endpoint(addr, port);

        while (true) {    
            std::cout << "Connecting..." << std::endl;

            boost::system::error_code ec;
            isConnected_.store(false);
            while (!stop_) {
                std::cout << "Try to connect..." << std::endl;
                socket_.connect(endpoint, ec);
                if (!ec) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }

            if (stop_) {
                return;
            }

            isConnected_.store(true);
            std::cout << "Connected" << std::endl;

            read();
            context_.run();

            std::cout << "Disconnected" << std::endl;

            socket_.close();
            context_.reset();
        }

        std::cout << "Exit" << std::endl;
    });
}

TcpClient::~TcpClient() {
    stop_ = true;
    thread_.join();
}

void TcpClient::read() {
    socket_.async_read_some(boost::asio::buffer(buffer_, std::size(buffer_)), [this] (const boost::system::error_code& err, std::size_t length) {
        if (!err) {
            buffer_[length] = '\0';
            if (callback_) {
                callback_(buffer_);
            }
            read();
        }
    });
}

void TcpClient::write(const std::string& message) {
    if (isConnected_.load()) {
        socket_.async_write_some(boost::asio::buffer(message, message.size()), [] (const boost::system::error_code& error, std::size_t bytes_transferred) {});
    }
}