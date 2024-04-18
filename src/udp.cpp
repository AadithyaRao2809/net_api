#ifndef UDP_F
#include "sock.cpp"
#include <iostream>

#define UDP_F
using namespace net;
namespace net {

template <typename T>
class Server;

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
    socklen_t client_addr_len = sizeof(client_addr);
    friend class Server<UDPServer<T, PORT>>;

    public:


    void setResponse(string response){
        send_str = response;
    }

    string getRequest(){
        return recv_str;
    }
    //Server function to read messages from the client
    int read() {
        recv_str.resize(1024);
        int n = ::recvfrom(this->socket_fd, recv_str.data(), recv_str.size(), 0, &client_addr,
                           &client_addr_len);
        debug("Reading from server");
        if (n > 0)
            recv_str.resize(n);
        return n;
    }
    //Server function to write messages to the client, with a response
    int write(string response) {
        int n = ::sendto(this->socket_fd, response.data(), response.size(), 0, &client_addr,
                         client_addr_len);
        debug("Writing to client");
        return n;
    }
    // Server function to write default messages to the client
    int write() {
        int n = ::sendto(this->socket_fd, send_str.data(), send_str.size(), 0, &client_addr,
                         client_addr_len);
        debug("Writing to client");
        return n;
    }


    void startServer(){
        while(1){
            string response;
            int n = read();                 //Waiting for a read from the client

            if(n < 0) {
                debug("Error reading from client");
                return;
            }
            cout << "Client: " << recv_str << endl;
            /*Can include type traits here*/
            if (client_addr.sa_family == AF_INET)  {                     // Getting the IP address of the client for IPv4
            client_addrin_ptr = (struct sockaddr_in *)&client_addr;
            char *ip = inet_ntoa(client_addrin_ptr->sin_addr);
            std::cout << "Client IP Address is: " << ip << std::endl;
            }
            else if(client_addr.sa_family == AF_INET6)              // Getting the IP address of the client for IPv6
            {
                char ip[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&client_addr)->sin6_addr, ip, INET6_ADDRSTRLEN);
                std::cout << "Client IP Address is: " << ip << std::endl;
            }
            else
            {
                std::cout << "Unknown address family" << std::endl;
            }


            cout << "Enter the message to send: ";      // Sending a message to the client
            getline(cin, response);
            send_str = response;
            write();
        }
    }
    UDPServer(T ip) : UDPSocket<T, PORT>(ip) {
        if (bind(this->socket_fd, (struct sockaddr *)&this->servaddr, sizeof(this->servaddr)) ==
            -1) {
            perror("Bind failed");
            throw runtime_error("Socket bind failed");
        }
        debug("Socket bound");
    }
};

template <IP T, int PORT>
class UDPClient : UDPSocket<T, PORT> {

    struct sockaddr server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    string send_str;
    string recv_str;

    public:

    void setRequest(string request){
        send_str = request;
    }

    string getResponse(){
        return recv_str;
    }
    //Client function that waits for a message from the server
    int read(){
        recv_str.resize(1024);
        debug("Reading from server");
        int n = ::recvfrom(this->socket_fd, recv_str.data(), recv_str.size(), 0, &server_addr,
                           &server_addr_len);
        if (n > 0)
            recv_str.resize(n);
        return n;
    }

    //Client function that writes a message to the Server, with a response
    int write(string response){
        debug("Response: " + response);
        int n = ::sendto(this->socket_fd, response.data(), response.size(), 0, &server_addr,
                         server_addr_len);
        if (n < 0) {
            debug("Error writing to server");
            return -1;
        }
        debug("Writing to server");
        return n;
    }
    //Client function that writes a default message to the Server
    int write(){
        int n = ::sendto(this->socket_fd, send_str.data(), send_str.size(), 0, &server_addr, server_addr_len);
        debug("Writing to server");
        return n;
    }

    // Client function to send and receive messages


    // Client constructor
    UDPClient(T ip) : UDPSocket<T, PORT>(ip) {
        if (is_same_v<T, IPv4>) {
            server_addr_len = sizeof(struct sockaddr_in);
            ((struct sockaddr_in *)&server_addr)->sin_family = AF_INET;
            ((struct sockaddr_in *)&server_addr)->sin_port = htons(PORT);
            ((struct sockaddr_in *)&server_addr)->sin_addr.s_addr = ip.address;
        } else {
            server_addr_len = sizeof(struct sockaddr_in6);
            ((struct sockaddr_in6 *)&server_addr)->sin6_family = AF_INET6;
            ((struct sockaddr_in6 *)&server_addr)->sin6_port = htons(PORT);
            memcpy(&((struct sockaddr_in6 *)&server_addr)->sin6_addr, &ip.address,
                   sizeof(ip.address));
        }
    }
};

template <IP T, int PORT>
class Server<UDPServer<T,PORT>> {
    UDPServer<T,PORT> server;

  public:
    Server(auto ip) : server(T(ip)) {
    }
    void start() {
        while (1) {
            server.read();
            // send_str is private 
            server.send_str = "Hello from server\n";
            server.write();
        }
    }
};

} // namespace net
#endif
