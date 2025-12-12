#include "tcp_server.h"

#include <iostream>

#include "tcp_connection.h"

tcp_server::tcp_server(boost::asio::io_context& io_context, unsigned short port) :
    io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    std::cout << "[Server] Listener started on port " << port << '\n';
}

void tcp_server::start_accept() { do_accept(); }

void tcp_server::do_accept() {
    auto new_socket = std::make_shared<tcp::socket>(io_context_);

    acceptor_.async_accept(*new_socket, [self = shared_from_this(),
                                    new_socket](const boost::system::error_code& error) {
        auto server = std::static_pointer_cast<tcp_server>(self);
        if (!error) {
            auto new_connection = std::make_shared<tcp_connection>(std::move(*new_socket), server);
            server->connections_.insert(new_connection);
            new_connection->start();
        } else {
            std::cerr << "Accept error: " << error.message() << '\n';
        }
        server->do_accept();
    });
}

void tcp_server::do_broadcast(const std::string& message, std::shared_ptr<tcp_connection> sender) {
    std::string full_msg = "[Client " + sender->id_ + "]: " + message;
    std::cout << "[Broadcast] Sending: " << full_msg << std::endl;

    for (const auto& session: connections_) {
        if (session != sender) {
            session->deliver(full_msg);
        }
    }
}

void tcp_server::OnMessageReceived(std::shared_ptr<tcp_connection> session, const std::string& message) {
    do_broadcast(message, session);
}

void tcp_server::OnDisconnected(std::shared_ptr<tcp_connection> session) {
    session->close();
    connections_.erase(session);
    std::cout << "[Server] Session " << session->id_ << " removed. Active sessions: " << connections_.size() << '\n';
}
