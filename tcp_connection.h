#ifndef MESSENGER_SERVER_TCP_CONNECTION_H
#define MESSENGER_SERVER_TCP_CONNECTION_H

#include <string>
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection> {
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    explicit tcp_connection(boost::asio::io_context& io_context);
    static pointer create(boost::asio::io_context& io_context);
    void start();

    tcp::socket& socket() {
        return socket_;
    }
    
private:
    tcp::socket socket_;
    std::string message_;

    void handle_write();
    void handle_read();
};


#endif //MESSENGER_SERVER_TCP_CONNECTION_H