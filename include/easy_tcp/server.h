#pragma once
#include <easy_tcp/connection.h>
#include <easy_tcp/listener.h>
#include <easy_tcp/service.h>
#include <vector>

namespace easy_tcp{
    template <class T>
    struct Server{
        static_assert(std::is_base_of<Service, T>::value, "T must inherit from Service");

        bool start(int port){
            listener.start(port);
            listening = new std::atomic<bool>;
            *listening = false;
            incoming_connections = new std::thread([this] () {
                *listening = true;
                while (*listening){
                    auto incoming_connection = listener.wait_for_client(10);
                    if (incoming_connection>=0){
                        auto new_service = new T(incoming_connection);
                        clients.push_back(new_service);
                        new_service->on_connect();
                    }
                }

            });
            while (!*listening);
        }

        ~Server(){
            *listening = false;
            for(auto client:clients)
                delete(client);
            incoming_connections->join();
            delete(listening);
            delete(incoming_connections);
        }

        Listener listener;
    private:
        std::atomic<bool> *listening = nullptr;
        std::thread *incoming_connections =nullptr;
        std::vector<Service *> clients;
    };
}