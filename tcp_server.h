#ifndef MESSENGER_SERVER_TCP_SERVER_H
#define MESSENGER_SERVER_TCP_SERVER_H

#include <boost/asio.hpp>
#include <set>

using boost::asio::ip::tcp;

class tcp_connection;

class IConnectionObserver {
public:
    virtual void OnMessageReceived(std::shared_ptr<tcp_connection> connection, const std::string& message) = 0;
    virtual ~IConnectionObserver() = default;
    virtual void OnDisconnected(std::shared_ptr<tcp_connection> connection) = 0;
};

class tcp_server : public IConnectionObserver, public  std::enable_shared_from_this<tcp_server> {
public:
    explicit tcp_server(boost::asio::io_context& io_context, unsigned short port);

    void start_accept();

    void OnMessageReceived(std::shared_ptr<tcp_connection> connection, const std::string& message) override;
    void OnDisconnected(std::shared_ptr<tcp_connection> connection) override;


private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::set<std::shared_ptr<tcp_connection>> connections_;

    void do_accept();
    void handle_accept(std::shared_ptr<tcp_connection> new_session, const boost::system::error_code& error);

    void do_broadcast(const std::string& message, std::shared_ptr<tcp_connection> sender);
};

#endif  // MESSENGER_SERVER_TCP_SERVER_H
