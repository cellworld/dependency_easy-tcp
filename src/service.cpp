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
//            connection->set_no_block();
            ready = true;
            while(true){
                if (service.connection->receive_data()){
//                    char *copied= (char*) malloc(connection->received_data_size);
//                    memcpy(copied,connection->buffer,connection->received_data_size);
//                    on_incoming_data(copied,connection->received_data_size);
                    service.on_incoming_data(service.connection->buffer,service.connection->received_data_size);
//                    delete(copied);
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

    void Service::stop() {
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

    void Service::on_incoming_data(const char *data, int s) {
        int b = 0, e = 0;
        while (e < s) {
            for ( e = b; data[e] && e < s; e++);
            on_incoming_data(std::string(data + b));
            b = e + 1;
        }
    }

    void Service::on_disconnect() {

    }

    void Service::on_incoming_data(const std::string &) {

    }

}