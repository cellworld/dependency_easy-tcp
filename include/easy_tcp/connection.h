#pragma once
#include <cstring>
#include<string>
#include<vector>
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
            return true;
        }
    };
}