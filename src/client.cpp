#include <easy_tcp/client.h>

using namespace std;

namespace easy_tcp {
    Client::Client() : Connection(-1) {

    }

    bool Client::connect(const std::string &ip, int port) {
        try {
            auto conn = Connection::connect_remote(ip, port);
            file_descriptor = conn.file_descriptor;
        } catch (...){
            return false;
        }
        running = true;
        client_thread = thread([&] {
            while(running) {
                if (receive_data()) {
                    received_data(buffer, received_data_size);
                }
            }
        });
        return true;
    }

    bool Client::send_data(const std::string &d) {
        return Connection::send_data(d.c_str(), d.size() + 1);
    }

    void Client::received_data(const std::string &) {

    }

    void Client::received_data(char *buff, size_t size) {
        size_t s = 0, e = 0; // start and end set to zero
        while (e<size) { // while the end is before the size
            for (e = s; buff[e]; e++); // move the end to the first zero char
            if (e > s)
            received_data(string(buff + s)); // convert the c-string from s-e to std::string and call receive_data(string)
            s = e + 1; // move the start to end + 1
        }
    }

    void Client::disconnect() {
        running = false;
        Connection::disconnect();
        client_thread.join();
    }

    Client::~Client() {

    }
}