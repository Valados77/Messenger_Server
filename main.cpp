#include <iostream>
#include "tcp_server.h"

using boost::asio::ip::tcp;


int main() {
    try {
        boost::asio::io_context context;
        tcp_server server(context);
        context.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}