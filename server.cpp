#include "server.h"

TcpServer::TcpServer(io_context& iocontext, int port) : acceptor_(iocontext, tcp::endpoint(tcp::v4(), port)) {

}

void TcpServer::start(DataCallback callback) {
    accept(callback);
}

void TcpServer::accept(DataCallback callback) {
    acceptor_.async_accept([this, callback](boost::system::error_code err, tcp::socket socket) {
        if (!err) {
            std::make_shared<Session>(std::move(socket), callback)->read();
        }

        accept(callback);
    });
}

TcpServer::Session::Session(tcp::socket socket, DataCallback callback) : socket_(std::move(socket)), callback_(callback) {

}

void TcpServer::Session::read() {
    auto self(shared_from_this());
    socket_.async_read_some(buffer(buffer_, std::size(buffer_)), [this, self] (boost::system::error_code err, std::size_t length) {
        if (!err) {
            buffer_[length] = '\0';
            
            if (callback_) {
                std::string response;
                callback_(buffer_, response);
                socket_.async_write_some(buffer(response, response.size()), [this, self] (boost::system::error_code err, std::size_t length) {});
            }
            read();
        }
    });
}
