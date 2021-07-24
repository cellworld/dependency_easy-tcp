#include<catch.h>
#include <easy_tcp.h>

using namespace std;
using namespace easy_tcp;

bool test_message_received_callback_client (const char*buffer, unsigned int size){
    cout << "client: " << size << ":" << buffer << endl;
    return true;
}

bool test_message_received_callback_server (const char*buffer, unsigned int size){
    cout << "server: " << size << ":" << buffer << endl;
    return true;
}


vector<Connection> connections;

bool test_listener_callback(int fd, string ip){
    cout << "Connection attempt from " << ip << " with descriptor " << fd << endl;
    auto &cnn = connections.emplace_back(fd, ip, test_message_received_callback_server);
    cnn.start();
    cnn.send_data("HELLO!");
    return true;
}


TEST_CASE("Listener basic constructor") {
    Listener listener;
    CHECK(!listener.start());
    listener.port = 8080;
    listener.queue_size = 1;
    listener.call_back = test_listener_callback;
    CHECK(listener.start());
}

TEST_CASE("Listener full constructor") {
    Listener listener(8080,test_listener_callback);
    CHECK(listener.start());
}

TEST_CASE("Connection client") {
    Listener listener(8080,test_listener_callback);
    CHECK_NOTHROW(listener.start());
    Connection cnn("127.0.0.1",8080,test_message_received_callback_client);
    sleep(1);
    cnn.start();
    sleep(1);
    CHECK(connections.size() == 1);
    sleep(1);
    cnn.send_data("hello!");
    sleep(1);
    cout << "removing connections" << endl;
    connections.clear();
    cout << "stopping client" << endl;
    cnn.stop();
    connections[0].stop();
    listener.stop();
}