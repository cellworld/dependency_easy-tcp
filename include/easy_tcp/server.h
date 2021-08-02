#pragma once
#include <easy_tcp/connection.h>
#include <easy_tcp/listener.h>
#include <easy_tcp/service.h>
#include <vector>
#include <iostream>

namespace easy_tcp{
    template <class T>
    struct Server{
        static_assert(std::is_base_of<Service, T>::value, "T must inherit from Service");

        bool start(int port){
            if (!listener.start(port)) return false;
            listening = new std::atomic<bool>;
            *listening = false;
            incoming_connections_thread = new std::thread([this] () {
                *listening = true;
                while (*listening){
                    auto incoming_connection = listener.wait_for_client(0);
                    if (incoming_connection >= 0){
                        auto new_service = new T();
                        clients.push_back(new_service);
                        new_service->start(incoming_connection);
                    }
                }

            });
            while (!*listening);
            return true;
        }

        ~Server(){
            *listening = false;
            incoming_connections_thread->join();
            for(auto client:clients) {
                client->stop();
                delete (client);
            }
            delete(listening);
            delete(incoming_connections_thread);
        }

        Listener listener;
    private:
        std::atomic<bool> *listening = nullptr;
        std::thread *incoming_connections_thread = nullptr;
        std::vector<Service *> clients;
    };
}