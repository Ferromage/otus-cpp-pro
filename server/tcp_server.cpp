#include "tcp_server.h"
#include <iostream>

using namespace tcpserver;

Server::Server(io_context& iocontext, int port) : acceptor_(iocontext, tcp::endpoint(tcp::v4(), port)) {

}

void Server::start(DataCallback callback) {
    accept(callback);
}

void Server::accept(DataCallback callback) {
    acceptor_.async_accept([this, callback](boost::system::error_code err, tcp::socket socket) {
        if (!err) {
            std::make_shared<Session>(std::move(socket), callback, id_++)->read();
        }

        accept(callback);
    });
}

Session::Session(tcp::socket socket, DataCallback callback, int id) : socket_(std::move(socket)), callback_(callback), id_(id) {

}

Session::~Session() {
        
}

void Session::read() {
    auto self(shared_from_this());

    socket_.async_read_some(buffer(buffer_, std::size(buffer_)), [this, self] (boost::system::error_code err, std::size_t length) {
        if (!err) {
            buffer_[length] = '\0';
            
            if (callback_) {
                std::string response;
                callback_(buffer_, response, self, false, id_);

                if (!response.empty()) {
                    socket_.async_write_some(buffer(response, response.size()), [this, self] ([[maybe_unused]] boost::system::error_code err, [[maybe_unused]] std::size_t length) {});
                }
            }
            read();
        } else {
            std::string response;
            callback_("", response, self, true, id_);
        }
    });
}

void Session::write(const std::string& message) {

}
