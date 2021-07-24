#pragma once
#include <thread>

namespace easy_tcp {
    typedef bool (connection_attempt_func)(int, std::string);
    typedef connection_attempt_func *connection_attempt_func_t;

    struct Listener {
        Listener ();
        Listener (int port, connection_attempt_func_t call_back, int queue_size = 10);
        bool start();
        bool start(int port, connection_attempt_func_t call_back, int queue_size = 10);
        void stop();

        int port;
        int file_descriptor;
        bool active;
        connection_attempt_func_t call_back;
        int queue_size;
        std::thread loop_thread;
        ~Listener();
    };
}