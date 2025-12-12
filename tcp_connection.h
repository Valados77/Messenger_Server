#ifndef MESSENGER_SERVER_TCP_CONNECTION_H
#define MESSENGER_SERVER_TCP_CONNECTION_H

#include <boost/asio.hpp>
#include <deque>
#include <memory>
#include <string>

class IConnectionObserver;
class tcp_server;

using boost::asio::ip::tcp;

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    explicit tcp_connection(tcp::socket socket, std::shared_ptr<IConnectionObserver> observer);
    void start();
    void close();

    void deliver(const std::string& msg);

    std::string id_;
private:
    tcp::socket socket_;
    std::shared_ptr<IConnectionObserver> observer_;
    boost::asio::streambuf buffer_;
    std::deque<std::string> write_msgs_;

    void do_read();
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void do_write();
    void handle_write(const boost::system::error_code& error);
};


#endif  // MESSENGER_SERVER_TCP_CONNECTION_H
