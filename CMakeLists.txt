cmake_minimum_required(VERSION 4.0)
project(Messenger_Server)

set(CMAKE_CXX_STANDARD 20)

add_executable(Messenger_Server main.cpp
        tcp_server.cpp
        tcp_server.h
        tcp_connection.cpp
        tcp_connection.h)
