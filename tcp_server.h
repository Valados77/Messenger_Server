#ifndef MESSENGER_SERVER_TCP_SERVER_H
#define MESSENGER_SERVER_TCP_SERVER_H

#include <boost/asio.hpp>
#include "tcp_connection.h"

using boost::asio::ip::tcp;

class tcp_server {
public:
    explicit tcp_server(boost::asio::io_context& context);

private:
    boost::asio::io_context& context_;
    tcp::acceptor acceptor_;

    void start_accept();
    void handle_accept(const tcp_connection::pointer& new_connection,
        const boost::system::error_code& err);
};


#endif //MESSENGER_SERVER_TCP_SERVER_H