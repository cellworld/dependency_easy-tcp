#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <functional>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <signal.h>
#include <fcntl.h>
#include <easy_tcp/listener.h>

using namespace std;

namespace easy_tcp{

    int process_client_connection_attempt(int m_sockfd, uint timeout, string &client_ip_address) {

        if (timeout>0) {
            timeval tv;
            fd_set m_fds;
            tv.tv_sec = timeout;
            tv.tv_usec = 0;
            FD_ZERO(&m_fds);
            FD_SET(m_sockfd, &m_fds);
            int selectRet = select(m_sockfd + 1, &m_fds, NULL, NULL, &tv);
            if (selectRet == -1) { // select failed
                cerr << strerror(errno) << endl;
                return -1;
            } else if (selectRet == 0) { // timeout
                return -1;
            } else if (!FD_ISSET(m_sockfd, &m_fds)) { //no new client
                return -1;
            }
        }
        sockaddr_in m_clientAddress;
        socklen_t so_size = sizeof(m_clientAddress);
        int file_descriptor = accept(m_sockfd, (struct sockaddr *) &m_clientAddress, &so_size);
        if (file_descriptor == -1) { // accept failed
            cerr << strerror(errno) << endl;
            return -1;
        }
        client_ip_address = inet_ntoa(m_clientAddress.sin_addr);
        return file_descriptor;
    }

    void process_incoming_connection_requests(Listener *listener){
        cout << "listening on port " << listener->port << endl;
        while (listener->active && (fcntl(listener->file_descriptor, F_GETFD) != -1 || errno != EBADF)){
            string client_ip_address;
            int file_descriptor = process_client_connection_attempt(listener->file_descriptor,1, client_ip_address);
            if (file_descriptor > 0)
                if (!listener->call_back(file_descriptor, client_ip_address)) close(file_descriptor);
        }
    }

    bool Listener::start() {
        if (port == -1) { //port not set
            cerr << "port number not set" << endl;
            return false;
        }
        struct sockaddr_in m_serverAddress;
        file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (file_descriptor == -1) { //socket failed
            cerr << "socket failed" << endl;
            return false;
        }
        int option = 1;
        setsockopt(file_descriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        memset(&m_serverAddress, 0, sizeof(m_serverAddress));
        m_serverAddress.sin_family = AF_INET;
        m_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        m_serverAddress.sin_port = htons(port);
        int bindSuccess = bind(file_descriptor, (struct sockaddr *) &m_serverAddress, sizeof(m_serverAddress));
        if (bindSuccess == -1) { // bind failed
            cerr << "bind failed" << endl;
            return false;
        }
        int listenSuccess = listen(file_descriptor, queue_size);
        if (listenSuccess == -1) { // listen failed
            cerr << "listen failed" << endl;
            return false;
        }

        active = true;
        loop_thread = thread(&process_incoming_connection_requests, this);
        return true;
    }

    void Listener::stop() {
        active = false;
        loop_thread.join();
        close(file_descriptor);
    }

    Listener::Listener(int port, connection_attempt_func_t call_back, int queue_size) : port(port), file_descriptor(0), active(false), call_back(call_back), queue_size(queue_size){
    }

    Listener::Listener() : Listener(-1, nullptr, 0){

    }

    bool Listener::start(int p_port, connection_attempt_func_t p_call_back, int p_queue_size) {
        this->port = p_port;
        this->call_back = p_call_back;
        this->queue_size = p_queue_size;
        return start();
    }

    Listener::~Listener() {
        stop();
    }
}

