#include "tcp_connection.h"

#include <iostream>
#include <string>

tcp_connection::tcp_connection(boost::asio::io_context& io_context)
    : socket_(io_context) { }

tcp_connection::pointer tcp_connection::create(boost::asio::io_context& io_context) {
    return std::make_shared<tcp_connection>(io_context);
}

void tcp_connection::start() {
    message_.resize(1024);
    handle_read();
    handle_write();

}

void tcp_connection::handle_write(){
    auto same = shared_from_this();
    /*std::cin >> message_;
    boost::asio::async_write(socket_, boost::asio::buffer(message_),
        [same](const boost::system::error_code& err, size_t size){
            same->handle_write();
        });*/

    std::thread([same]() {
        std::string text;
        while (std::getline(std::cin, text)) {
            boost::asio::post(same->socket_.get_executor(),
                [same, text]() {
                    boost::asio::async_write(
                        same->socket_,
                        boost::asio::buffer(text),
                        [same](auto, auto){});
            });
        }
    }).detach();
}

void tcp_connection::handle_read() {
    auto same = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(message_.data(), message_.size()),
        [same](const boost::system::error_code& err, size_t size) {
            if (!err) {
                std::cout << "Получено: "
                          << same->message_.substr(0, size)
                          << std::endl;
            same->handle_read();
            }
        });
}

