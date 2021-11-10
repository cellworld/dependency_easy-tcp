#include<catch.h>
#include <easy_tcp.h>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace easy_tcp;

std::atomic<bool> received = false;

struct Test_service : Service {
    void on_connect() override {
        cout << "connected" << endl;
    }
    void on_disconnect() override {
        cout << "disconnected" << endl;
    }
    void on_incoming_data (const char* data, int size) override{
        cout << size << ":" << data << endl;
        received = true;
    }
};

TEST_CASE("server") {
    {
        Server<Test_service> server;

        if (!server.start(6700))
            cout << "Error opening port!" << endl;
        else {
            auto client = Connection::connect_remote("127.0.0.1", 6700);
            client.send_data("test", 5);
            cout << "data sent "<< endl;
        }
        while(!received) ;
    }
}

struct Test_client : Client {
    void received_data(const std::string &data) override{
        cout << data << endl;
    }
    void test () {
        for (int i=0;i<10; i++){
            buffer[i*3] = 'a' + i * 2 ;
            buffer[i*3+1] = 'a' + i * 2 + 1;
            buffer[i*3+2] = 0;
        }
        Client::received_data(buffer,30);
    }
};

TEST_CASE ("client") {
    Test_client tc;
    tc.test();
}