#pragma once
#include <easy_tcp/connection.h>
#include <thread>

namespace easy_tcp{
    struct Service {
        Service(int);
        virtual void on_connect();
        virtual void on_incoming_data(const char *, int);
        virtual void on_disconnect();
        Connection connection;
        ~Service();
    private:
        void process_incoming_data();
        std::atomic<bool> *receiving_data;
        std::thread *incoming_data_thread;
    };
}