#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <thread>
#include <fcntl.h>
#include <easy_tcp/connection.h>

using namespace std;


namespace easy_tcp {

    Connection::Connection (){

    }

    Connection::Connection(int file_descriptor, string ip, message_received_func_t message_received_call_back): file_descriptor(file_descriptor), remote_ip(std::move(ip)), remote_port(-1), message_received_call_back(message_received_call_back), active(false) {

    }

    Connection::Connection(string ip, int port, message_received_func_t message_received_call_back): file_descriptor(-1), remote_ip(std::move(ip)), remote_port(port), message_received_call_back(message_received_call_back), active(false) {

    }

    void process_received_data(Connection *conn){
        fcntl(conn->file_descriptor, F_SETFL, O_NONBLOCK);
        while (conn->active && (fcntl(conn->file_descriptor, F_GETFD) != -1 || errno != EBADF)) {
            char msg[MAX_PACKET_SIZE];
            int numOfBytesReceived = recv(conn->file_descriptor, msg, MAX_PACKET_SIZE, 0);
            if(numOfBytesReceived < 1) {
                if (numOfBytesReceived == 0) { //server closed connection
                    cerr <<  "Server closed connection" << endl;
                    conn->active = false;
                    close(conn->file_descriptor);
                }
            } else {
                if (!conn->message_received_call_back(msg, numOfBytesReceived)) {
                    conn->active = false;
                    close(conn->file_descriptor);
                }
            }
        }
    }

    void Connection::stop() {
        active = false;
        close(file_descriptor);
        if (loop_thread) {
            cout << "stopping connection :" << file_descriptor << endl;
            loop_thread->join();
            delete(loop_thread);
            loop_thread = nullptr;
        }
    }

    Connection::~Connection() {
        stop();
    }

    bool Connection::start() {
        if (remote_port >= 0) {
            cout << "Connection: connecting to remote server " << remote_ip << endl;
            struct sockaddr_in m_server;

            file_descriptor = socket(AF_INET , SOCK_STREAM , 0);
            if (file_descriptor == -1) { //socket failed
                cerr << "Connection: socket creation failed" << endl;
                return false;
            }

            int inetSuccess = inet_aton(remote_ip.c_str(), &m_server.sin_addr);

            if(!inetSuccess) { // inet_addr failed to parse address
                // if hostname is not in IP strings and dots format, try resolve it
                struct hostent *host;
                struct in_addr **addrList;
                if ( (host = gethostbyname( remote_ip.c_str() ) ) == NULL){
                    cerr << "Connection: failed to resolve host name" << endl;
                    return false;
                }
                addrList = (struct in_addr **) host->h_addr_list;
                m_server.sin_addr = *addrList[0];
            }
            m_server.sin_family = AF_INET;
            m_server.sin_port = htons( remote_port );

            int connectRet = connect(file_descriptor , (struct sockaddr *)&m_server , sizeof(m_server));
            if (connectRet == -1) {
                cerr << strerror(errno) << endl;
                return false;
            }
        }
        active = true;
        cout << "starting connection :" << file_descriptor << endl;
        loop_thread = new thread(&process_received_data, this);
        return true;
    }

    bool Connection::send_data(const char *message, unsigned int size) {
        int bytes_sent = send(file_descriptor, (char *)message, size, 0);
        if (bytes_sent == (int) size) return true;
        return false;
    }

    bool Connection::send_data(std::string message) {
        return send_data(message.c_str(),message.size());
    }

}