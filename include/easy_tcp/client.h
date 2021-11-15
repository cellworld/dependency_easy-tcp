#pragma once
#include <easy_tcp/connection.h>
#include <atomic>
#include <thread>

namespace easy_tcp {
    struct Client : Connection {
        Client();
        ~Client();
        bool connect (const std::string &, int port);
        bool send_data (const std::string &);
        virtual void received_data(const std::string &);
        virtual void received_data(char *, size_t);
        void disconnect();
        std::atomic<bool> running;
        std::thread client_thread;
    };
}