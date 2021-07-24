#pragma once

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <functional>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <easy_tcp/listener.h>
#include <easy_tcp/service.h>

#define MAX_PACKET_SIZE 4096

namespace easy_tcp {
    template <class T>
    requires std::is_base_of<Service, T>::value
    class Server {
    public:
        bool start(){
            clients.clear();
            return true;
        };
        bool finish();
    private:
        std::vector<T> clients;
    };
}
