//
// Created by vladislav on 05.12.2025.
//

#include "tcp_server.h"

tcp_server::tcp_server(boost::asio::io_context& context)
        : context_(context),
    acceptor_(context, tcp::endpoint(tcp::v4(), 13)) {
    start_accept();
}

void tcp_server::start_accept() {
    tcp_connection::pointer new_connection = tcp_connection::create(context_);
    acceptor_.async_accept(new_connection->socket(),
        [this, new_connection](const boost::system::error_code& err) {
            handle_accept(new_connection, err);
        });
}

void tcp_server::handle_accept(const tcp_connection::pointer& new_connection,
    const boost::system::error_code &err) {
    if (!err)
        new_connection->start();
    start_accept();
}
