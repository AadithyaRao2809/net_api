#ifndef SOCK_F
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "debug.cpp"
#include "ip.cpp"

#define SOCK_F

namespace net {
using namespace std;

template <typename T>
concept IP = is_same_v<T, IPv4> || is_same_v<T, IPv6>;                  //Use of concept to constrain the type of IP to IPv4 or IPv6


// Socket class which will be used by the UDPSocket and TCPSocket classes

template <IP T, int PORT>
class Socket {
    T ip_addr;

  protected:
    conditional_t<is_same_v<T, IPv4>, sockaddr_in, sockaddr_in6> servaddr, clientaddr;          //Type trait to assign the correct sockaddr structure based on the IP type

    // Function to create a socket
    void createSocket(){
        if constexpr(is_same_v<T, IPv4>){
            if((socket_fd = socket(AF_INET, protocol(), 0)) == -1){
                throw runtime_error("Socket creation failed");
            }
            debug("Socket created");
            int optval = 1;
            setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
        }
        else{
            if((socket_fd = socket(AF_INET6, protocol(), 0)) == -1){
                throw runtime_error("Socket creation failed");
            }
            debug("Socket created");
            int optval = 1;
            setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
        }
    };

  public:

    int socket_fd = -1;
    virtual int protocol() = 0;                 // Virtual function to be implemented by the derived classes
    Socket(T ip) : ip_addr(ip) {
        // Socket creation based on whether the network protocol is IPv4 or IPv6
        if constexpr (std::is_same_v<T, IPv4>) /* Check if IP is v4 */
        {
            servaddr.sin_family = AF_INET; // AF_INET is for IPv4
            servaddr.sin_addr.s_addr = ip_addr.address;
            servaddr.sin_port = htons(PORT);
            debug("Port set " + to_string(PORT));

        } else if constexpr (std::is_same_v<T, IPv6>) /* Check if IP is v6 */
        {
            debug("IPv6 scope entered");
            servaddr.sin6_family = AF_INET6; // AF_INET6 is for IPv6
            memcpy(&servaddr.sin6_addr, &ip_addr.address, sizeof(ip_addr.address));
            servaddr.sin6_port = htons(PORT);

            debug("Port set", to_string(PORT));
           
        } else
            throw invalid_argument("Invalid IP type");
    }

    ~Socket() { close(socket_fd); }
    virtual int read() = 0;
    virtual int write() = 0;
};

} // namespace net
#endif
