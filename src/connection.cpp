#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <easy_tcp/connection.h>

using namespace std;

namespace easy_tcp {

    Connection::Connection(int file_descriptor) : file_descriptor(file_descriptor), received_data_size(-1), state(Connection_state::Open){
    }

    bool Connection::send_data(const char *data, unsigned int size) {
        int bytes_sent = 0;
        try {
            bytes_sent = send(file_descriptor, (char *) data, size, 0);
        } catch (...){
            return false;
        }
        if (bytes_sent == (int) size) return true;
        return false;
    }

    void Connection::set_no_block() {
        fcntl(file_descriptor, F_SETFL, O_NONBLOCK);
    }

    Connection Connection::connect_remote(std::string remote_ip, int remote_port) {
        sockaddr_in m_server;

        int file_descriptor = socket(AF_INET , SOCK_STREAM , 0);
        if (file_descriptor == -1) { //socket failed
            throw logic_error("Connection: socket creation failed");
        }
        int inetSuccess = inet_aton(remote_ip.c_str(), &m_server.sin_addr);
        if(!inetSuccess) { // inet_addr failed to parse address
            // if hostname is not in IP strings and dots format, try resolve it
            struct hostent *host;
            struct in_addr **addrList;
            if ( (host = gethostbyname( remote_ip.c_str() ) ) == NULL){
                throw logic_error("Connection: failed to resolve host name");
            }
            addrList = (struct in_addr **) host->h_addr_list;
            m_server.sin_addr = *addrList[0];
        }
        m_server.sin_family = AF_INET;
        m_server.sin_port = htons( remote_port );

        int connectRet = connect(file_descriptor , (struct sockaddr *)&m_server , sizeof(m_server));
        if (connectRet == -1) {
            throw logic_error(strerror(errno));
        }
        return Connection(file_descriptor);
    }

    void Connection::disconnect() {
        state = Connection_state::Closed;
        shutdown(file_descriptor, SHUT_RDWR);
        close(file_descriptor);
    }

    bool Connection::receive_data() {
        if (fcntl(file_descriptor, F_GETFD) != -1 || errno != EBADF)
        {
            received_data_size = recv(file_descriptor, buffer, MAX_PACKET_SIZE, 0);
            if( received_data_size < 1 ) {
                if ( received_data_size == 0 ) {
                    state = Connection_state::Closed;
                    return false;
                }
                state = Connection_state::Error;
                return false;
            } else {
                return true;
            }
        }
        state = Connection_state::Closed;
        return false;
    }
}
