#pragma once

namespace easy_tcp{
    struct Listener {
        bool start(int);
        void stop();
        int wait_for_client(int);
        ~Listener();
        int file_descriptor;
        int queue_size = 100;
    };
}