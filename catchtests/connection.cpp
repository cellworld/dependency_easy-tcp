#include<catch.h>
#include <easy_tcp.h>


using namespace std;
using namespace easy_tcp;




bool test_message_received_callback (const char*buffer, unsigned int size){
    cout << size << ":" << buffer << endl;
    return true;
}

bool test_listener_callback(int fd, string ip){
    cout << "Connection attempt from " << ip << " with descriptor " << fd << endl;
    Connection cnn(fd, ip, test_message_received_callback);
    cnn.send_data("HELLO!");
    return false;
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
    Connection cnn("127.0.0.1",8080,test_message_received_callback);
    sleep(1);
    cnn.start();
    sleep(1);
    cnn.send_data("hello!");
    sleep(1);

}