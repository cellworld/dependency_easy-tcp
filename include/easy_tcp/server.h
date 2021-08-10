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

        Server(){
            listening = new std::atomic<bool>;
            *listening = false;
        }

        bool start(int port){
            if (*listening) return false;
            if (!listener.start(port)) return false;
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

        void stop(){
            if (!*listening) return;
            *listening = false;
            listener.stop();
            incoming_connections_thread->join();
            delete(incoming_connections_thread);
            incoming_connections_thread = nullptr;
            for(auto client:clients) {
                client->stop();
                delete (client);
            }
            clients.clear();
        }

        ~Server(){
            stop();
            delete(listening);
        }

        Listener listener;
    private:
        std::atomic<bool> *listening = nullptr;
        std::thread *incoming_connections_thread = nullptr;
        std::vector<Service *> clients;
    };
}