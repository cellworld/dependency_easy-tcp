#include <easy_tcp/service.h>
#include <thread>
#include <iostream>

using namespace std;

namespace easy_tcp {

    bool Service::send_data(const char *data, int size) {
        return connection->send_data(data, size);
    }

    bool Service::send_data(const std::string &data) {
        return send_data(data.c_str(), data.size());
    }

    void Service::start(int file_descriptor) {
        connection = new Connection(file_descriptor);
        if (incoming_data_thread) {
            throw logic_error("Service already running");
        }
        bool ready = false;
        incoming_data_thread = new thread([this, &ready](){
            connection->set_no_block();
            ready = true;
            while(true){
                if (connection->receive_data()){
                    on_incoming_data(connection->buffer,connection->received_data_size);
                }
                if (connection->state == Connection_state::Closed) {
                    on_disconnect();
                    break;
                }
            }
        });
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

    void Service::on_incoming_data(const char *data, int) {
        on_incoming_data(std::string(data));
    }

    void Service::on_disconnect() {

    }

    void Service::on_incoming_data(const std::string &) {

    }

}