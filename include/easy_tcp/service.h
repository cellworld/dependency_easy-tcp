#pragma once
#include <easy_tcp/connection.h>
#include <thread>

namespace easy_tcp{
    struct Service {
        void start(int);

        virtual void on_connect();
        virtual void on_incoming_data(const char *, int);
        virtual void on_incoming_data(const std::string &);
        virtual void on_disconnect();

        bool send_data(const char *, int);
        bool send_data(const std::string &);

        void stop();

    private:
        Connection *connection = nullptr;
        std::thread *incoming_data_thread = nullptr;
    };
}