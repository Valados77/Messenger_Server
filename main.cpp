#include <iostream>
#include "tcp_server.h"

using boost::asio::ip::tcp;

void worker_thread_func(std::shared_ptr<boost::asio::io_context> io_context) {
    io_context->run();
}

int main() {
    try {
        auto io_context = std::make_shared<boost::asio::io_context>();
        auto work_guard = boost::asio::make_work_guard(*io_context);

        const size_t num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> thread_pool;

        for (size_t i = 0; i < num_threads; ++i) {
            thread_pool.emplace_back(worker_thread_func, io_context);
        }

        std::cout << "[Main] Server started with " << num_threads << " threads.\n";

        unsigned short port = 8080;
        auto server = std::make_shared<tcp_server>(*io_context, port);
        server->start_accept();

        std::cout << "\n[Main] Waiting for clients on port 8080. Press Enter to stop...\n";
        std::cin.get();

        work_guard.reset();
        io_context->stop();

        for (auto& t : thread_pool) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}