#include<catch.h>
#include <easy_tcp.h>
#include <atomic>
#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;
using namespace easy_tcp;

atomic<bool> listening;
int new_client;

void listen() {
    Listener listener;
    listener.start(8080);
    listening = true;
    new_client = -1;
    while (listening) {
        int i = listener.wait_for_client(1);
        if (i != -1) new_client = i;
    }
}

TEST_CASE("listen"){
    listening = false;
    std::thread t(&listen);
    while(!listening);
    Connection::connect_remote("127.0.0.1",8080);
    while(new_client==-1);
    CHECK(new_client!=-1);
    listening = false;
    t.join();
}

TEST_CASE("communication"){
    listening = false;
    std::thread t(&listen);
    while(!listening);
    auto client = Connection::connect_remote("127.0.0.1",8080);
    while(new_client==-1);
    CHECK(new_client!=-1);

    Connection server(new_client);
    client.set_no_block();
    server.set_no_block();

    CHECK(!client.receive_data());
    CHECK(!server.receive_data());

    client.send_data((char *)"test", 5);
    while (!server.receive_data());
    CHECK(server.received_data_size == 5);
    server.send_data(server.buffer, server.received_data_size);
    while (!client.receive_data());
    CHECK(client.received_data_size == 5);
    CHECK(string(client.buffer) == "test");
    CHECK(string(server.buffer) == "test");
    listening = false;
    t.join();
}

struct Test_service : Service {
    explicit Test_service(int port):Service(port) {}
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


TEST_CASE("server"){
    Server<Test_service> server;
    server.start(8080);
    auto client = Connection::connect_remote("127.0.0.1",8080);
    client.send_data("test",5);
    usleep(10000);
}