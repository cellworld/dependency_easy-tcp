#include <easy_tcp/service.h>
#include <thread>

using namespace std;

namespace easy_tcp {
    Service::Service(int file_descriptor): connection(file_descriptor) {
        receiving_data = new atomic<bool>;
        *receiving_data = false;
        incoming_data_thread = new thread([this](){
            *receiving_data = true;
            connection.set_no_block();
            while(*receiving_data){
                if (connection.receive_data()){
                    on_incoming_data(connection.buffer,connection.received_data_size);
                }
            }
        });
        while (!*receiving_data);
    }

    void Service::on_connect() {

    }

    void Service::on_incoming_data(const char *, int) {

    }

    void Service::on_disconnect() {

    }

    Service::~Service() {
        *receiving_data = false;
        incoming_data_thread->join();
        on_disconnect();
        delete (receiving_data);
        delete (incoming_data_thread);
    }
}