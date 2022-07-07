#pragma once
#include<cstring>
#include<string>
#include<vector>
#include<chrono>
#define MAX_PACKET_SIZE 8192

namespace easy_tcp{
    enum Connection_state {
        Open,
        Closed,
        Error
    };
    struct Connection {
        static Connection connect_remote(std::string, int);
        explicit Connection (int);
        bool send_data( const char *, unsigned int );
        bool receive_data();
        void set_no_block();
        void disconnect();
        int file_descriptor;
        int received_data_size;
        char buffer[MAX_PACKET_SIZE + 1];
        Connection_state state;
        template <typename T>
        bool get_data(T &v){
            if (sizeof(T) != received_data_size) return false;
            std::memcpy((char *)&v, buffer, received_data_size);
            received_data_size = 0;
            return true;
        }
        template <typename T, typename TO>
        bool wait_for_data(T &v, TO time_out){
            auto limit = std::chrono::high_resolution_clock::now() + time_out;
            while (limit > std::chrono::high_resolution_clock::now()){
                if (get_data(v)) return true;
            }
            return false;
        }
    };
}