#include <easy_tcp/service.h>
#include <thread>
#include <iostream>
#include <cstring>

using namespace std;

namespace easy_tcp {

    bool Service::send_data(const char *data, int size) {
        return connection->send_data(data, size);
    }

    bool Service::send_data(const std::string &data) {
        return send_data(data.c_str(), data.size() + 1);
    }

    void Service::start(int file_descriptor) {
        connection = new Connection(file_descriptor);
        if (incoming_data_thread) {
            throw logic_error("Service already running");
        }
        bool ready = false;
        incoming_data_thread = new thread([&ready](Service &service){
            ready = true;
            while(true){
                if (service.connection->receive_data()){
                    service.on_incoming_data(service.connection->buffer,service.connection->received_data_size);
                }
                if (service.connection->state == Connection_state::Closed) {
                    service.on_disconnect();
                    break;
                }
            }
        }, std::ref(*this));
        while(!ready);
        on_connect();
    }

    void Service::disconnect() {
        if (incoming_data_thread) {
            connection->disconnect();
            incoming_data_thread->join();
            delete (incoming_data_thread);
            incoming_data_thread = nullptr;
            delete(connection);
            connection = nullptr;
        }
    }

    void Service::on_connect() {

    }

    void Service::on_incoming_data(const char *buff, int size) {
        size_t s = 0, e = 0; // start and end set to zero
        while (e<(size_t)size) { // while the end is before the size
            for (e = s; buff[e]; e++); // move the end to the first zero char
            if (e > s)
                on_incoming_data(string(buff + s)); // convert the c-string from s-e to std::string and call receive_data(string)
            s = e + 1; // move the start to end + 1
        }
    }

    void Service::on_disconnect() {

    }

    void Service::on_incoming_data(const std::string &) {

    }

    Service::~Service() {
        disconnect();
    }

    void Service::stop() {
        *server_running = false;
    }

}