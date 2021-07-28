#include<catch.h>
#include <easy_tcp.h>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace easy_tcp;

struct Test_service : Service {
    void on_connect() override {
        cout << "connected" << endl;
    }
    void on_disconnect() override {
        cout << "disconnected" << endl;
    }
    void on_incoming_data (const char* data, int size) override{
        cout << size << ":" << data << endl;
    }
};

TEST_CASE("server") {
    {
        Server<Test_service> server;

        if (!server.start(8000))
            cout << "Error opening port!" << endl;
        else {
            auto client = Connection::connect_remote("127.0.0.1", 8000);
            client.send_data("test", 5);
        }
    }

}