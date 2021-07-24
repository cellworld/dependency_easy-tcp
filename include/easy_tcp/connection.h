#pragma once
#include <string>
#include <thread>

#define MAX_PACKET_SIZE 8192

namespace easy_tcp{
    typedef bool (message_received_func)(const char*, unsigned int);
    typedef message_received_func *message_received_func_t;

    struct Connection{
        Connection(int file_descriptor, std::string ip, message_received_func_t = nullptr);
        Connection(std::string ip, int port, message_received_func_t = nullptr);
        bool start();
        void stop();
        bool send_data(const char *, unsigned int);
        bool send_data(std::string);
        int file_descriptor;
        std::string remote_host;
        std::string remote_ip;
        int remote_port;
        message_received_func_t message_received_call_back;
        std::thread loop_thread;
        bool active;
        ~Connection();
    };
}