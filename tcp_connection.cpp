#include "tcp_connection.h"

#include <iostream>
#include <string>

#include "tcp_server.h"

tcp_connection::tcp_connection(tcp::socket socket, std::shared_ptr<IConnectionObserver> observer) :
    socket_(std::move(socket)), observer_(std::move(observer)) {
    boost::system::error_code ec;
    auto ep = socket_.remote_endpoint(ec);
    if (!ec) {
        id_ = std::to_string(ep.port());
    } else {
        id_ = "unknown";
    }
};

void tcp_connection::start() {
    std::cout << "[Session " << id_ << "] Connected. Starting read loop.\n";
    do_read();
}

void tcp_connection::close() {
    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    }
}

void tcp_connection::deliver(const std::string& msg) {
    write_msgs_.push_back(msg + "\n");

    if (write_msgs_.size() > 1) {
        return;
    }
    do_write();
}

void tcp_connection::do_read() {
    boost::asio::async_read_until(
            socket_, buffer_, '\n',
            [self = shared_from_this()](const boost::system::error_code& error, size_t bytes_transferred) {
                self->handle_read(error, bytes_transferred);
            });
}

void tcp_connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        std::istream is(&buffer_);
        std::string line;
        std::getline(is, line);

        observer_->OnMessageReceived(shared_from_this(), line);

        do_read();

    } else if (error == boost::asio::error::eof || error == boost::asio::error::connection_reset) {
        std::cout << "[Session " << id_ << "] Disconnected gracefully.\n";
        observer_->OnDisconnected(shared_from_this());
    } else {
        std::cerr << "[Session " << id_ << "] Read error: " << error.message() << '\n';
        observer_->OnDisconnected(shared_from_this());
    }
}

void tcp_connection::do_write() {
    boost::asio::async_write(socket_, boost::asio::buffer(write_msgs_.front()),
                             [self = shared_from_this()](const boost::system::error_code& error,
                                                         size_t /*bytes_transferred*/) { self->handle_write(error); });
}

void tcp_connection::handle_write(const boost::system::error_code& error) {
    if (!error) {
        write_msgs_.pop_front();
        if (!write_msgs_.empty()) {
            do_write();
        }
    } else {
        std::cerr << "[Session " << id_ << "] Write error: " << error.message() << '\n';
        observer_->OnDisconnected(shared_from_this());
    }
}
