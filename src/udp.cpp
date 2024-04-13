#include "sock.cpp"
#include <iterator>


using namespace net;
template <IP T, int PORT>
class UDPSocket : public Socket<T, PORT>{

    public:
    UDPSocket(T ip) : Socket<T, PORT>(ip) {

        this->createSocket();
    }

    int protocol() {
        return SOCK_DGRAM;
    }
};

template <IP T, int PORT>
class UDPServer : public UDPSocket<T, PORT> {

    string send_str;
    string recv_str;
    struct sockaddr client_addr;
    struct sockaddr_in *client_addrin_ptr;
    socklen_t client_addr_len;

    // recvfrom() 
    int read() {
        char buffer[1024];
        int buflen = 1024;
        int n = ::recvfrom(this->socket_fd, buffer, buflen, 0, &client_addr, &client_addr_len);
        recv_str = string(buffer,n);
        return n;
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
        debug("Waiting for client");
        int n = read();
        buffer[n] = '\0';
        debug("Client: " + string(buffer));

        if (client_addr.sa_family == AF_INET)
         client_addrin_ptr = (struct sockaddr_in *)&client_addr;
         char *ip = inet_ntoa(client_addrin_ptr->sin_addr);
         std::cout << "Client IP Address is: " << ip << std::endl;
        }
    }

   };
