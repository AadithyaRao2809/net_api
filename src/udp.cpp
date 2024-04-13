#include "sock.cpp"
#include <iostream>
#include <iterator>

using namespace net;
template <IP T, int PORT>
class UDPSocket : public Socket<T, PORT> {

  public:
    UDPSocket(T ip) : Socket<T, PORT>(ip) { this->createSocket(); }

    int protocol() { return SOCK_DGRAM; }
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
        recv_str.resize(1024);
        int n = ::recvfrom(this->socket_fd, recv_str.data(), recv_str.size(), 0, &client_addr, &client_addr_len);
        debug("Reading from server");
        if(n>0) recv_str.resize(n);
        return n;
    }
    int write(string response) {
        int n = ::sendto(this->socket_fd, response.data(), response.size(), 0, &client_addr, client_addr_len);
        debug("Writing to client");
        return n;
    }
    int write(){
        int n = ::sendto(this->socket_fd, send_str.data(), send_str.size(), 0, &client_addr, client_addr_len);
        debug("Writing to client");
        return n;
    }

    public:

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
        debug("Client: " + recv_str);

            if (client_addr.sa_family == AF_INET)
                client_addrin_ptr = (struct sockaddr_in *)&client_addr;
            char *ip = inet_ntoa(client_addrin_ptr->sin_addr);
            std::cout << "Client IP Address is: " << ip << std::endl;
        }
    }

   };

template <IP T, int PORT>
class UDPClient : UDPSocket<T, PORT>{

    struct sockaddr server_addr;
    socklen_t server_addr_len;
    string send_str;
    string recv_str;

    public:

    int read(){
        recv_str.resize(1024);
        debug("Reading from server");
        int n = ::recvfrom(this->socket_fd, recv_str.data(), recv_str.size(), 0, &server_addr, &server_addr_len);
        if(n>0) recv_str.resize(n);
        return n;
    }
    int write(string response){
        debug("Response: " + response);
        int n = ::sendto(this->socket_fd, response.data(), response.size(), 0, &server_addr, server_addr_len);
        if(n < 0) {
            debug("Error writing to server");
            return -1;
        }
        debug("Writing to server");
        return n;
    }
    int write(){
        int n = ::sendto(this->socket_fd, send_str.data(), send_str.size(), 0, &server_addr, server_addr_len);
        debug("Writing to server");
        return n;
    }
    UDPClient(T ip) : UDPSocket<T, PORT>(ip) {
        if(is_same_v<T, IPv4>){
            server_addr_len = sizeof(struct sockaddr_in);
            ((struct sockaddr_in *)&server_addr)->sin_family = AF_INET;
            ((struct sockaddr_in *)&server_addr)->sin_port = htons(PORT);
            ((struct sockaddr_in *)&server_addr)->sin_addr.s_addr = ip.address;
        }
        else{
            server_addr_len = sizeof(struct sockaddr_in6);
            ((struct sockaddr_in6 *)&server_addr)->sin6_family = AF_INET6;
            ((struct sockaddr_in6 *)&server_addr)->sin6_port = htons(PORT);
            memcpy(&((struct sockaddr_in6 *)&server_addr)->sin6_addr, &ip.address, sizeof(ip.address));
        }
    }

};
