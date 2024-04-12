#include "sock.cpp"


using namespace net;
template <IP T, int PORT>
class UDPSocket : public Socket<T, PORT>{

    public:
    UDPSocket(T ip) : Socket<T, PORT>(ip) {

        createSocket<T>(ip);
    }

    int protocol() {
        return SOCK_DGRAM;
    }
};

template <IP T, int PORT>
class UDPServer : public UDPSocket<T, PORT> {

    char buffer[1024];
    int buflen = 1024;
    struct sockaddr client_addr;
    socklen_t client_addr_len;

    // recvfrom() 
    int read() {
        return ::recvfrom(this->socket_fd, buffer, buflen, MSG_WAITALL, &client_addr, &client_addr_len);
    }
    int write() {
        return 0;
    }

    public:

    UDPServer(T ip) : UDPSocket<T, PORT>(ip) {
    if (bind(this->socket_fd, (struct sockaddr *)&this->servaddr, sizeof(this->servaddr)) ==
            -1) {
            perror("Bind failed");
            throw runtime_error("Socket bind failed");
        }
        debug("Socket bound");

    while(1) {
        int n = read();
        buffer[n] = '\0';
        debug("Client: " + string(buffer));
        
        }
    }

   };