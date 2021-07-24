#pragma once

#include <string>
#include <thread>
#include <functional>

namespace easy_tcp {
    struct Service {
        virtual bool on_connect(std::string) {
            return true;
        };
        virtual void on_data_received(const char *msg, unsigned int) {
            std::string msg_str = msg;
            on_data_received(msg_str);
        };
        virtual void on_data_received(const std::string &) {};
        virtual void on_disconnect() {};
        ~Service();
        bool operator == (const Service &other);
        void set_handler(std::function<void(void)> func) { m_threadHandler = new std::thread(func); }
        int fd = 0;
        std::string ip;
        bool connected;
        std::thread *m_threadHandler = nullptr;
    };
}