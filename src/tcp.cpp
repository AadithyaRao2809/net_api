
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "debug.cpp"
#include "ip.cpp"

namespace net {
using namespace std;

template <typename T>
concept IP = is_same_v<T, IPv4> || is_same_v<T, IPv6>;

template <IP T, int PORT>
class TCPSocket {
    T ip_addr;

  protected:
    conditional_t<is_same_v<T, IPv4>, sockaddr_in, sockaddr_in6> servaddr, clientaddr;

  public:
    int socket_fd = -1;
    TCPSocket(T ip) : ip_addr(ip) {
        // Socket creation based on whether the network protocol is IPv4 or IPv6
        if constexpr (std::is_same_v<T, IPv4>) /* Check if IP is v4 */ {
            servaddr.sin_family = AF_INET; // AF_INET is for IPv4
            servaddr.sin_addr.s_addr = ip_addr.address;
            servaddr.sin_port = htons(PORT);

            debug("Port set " + to_string(PORT));
            if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) /* SOCK_STREAM is for TCP */ {
                throw runtime_error("Socket creation failed");
            }
            debug("Socket created");
            int optval = 1;
            setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

        } else if constexpr (std::is_same_v<T, IPv6>) /* Check if IP is v6 */ {
            debug("IPv6 scope entered");
            servaddr.sin6_family = AF_INET6; // AF_INET6 is for IPv6
            memcpy(&servaddr.sin6_addr, &ip_addr.address, sizeof(ip_addr.address));
            servaddr.sin6_port = htons(PORT);

            debug("Port set", to_string(PORT));
            if ((socket_fd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) /* SOCK_STREAM is for TCP */ {
                throw runtime_error("Socket creation failed");
            }
            debug("Socket created");
            int optval = 1;
            setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
        } else
            throw invalid_argument("Invalid IP type");

        // Binding socket to the IP and port
    }

    int close() { return ::close(socket_fd); }
    virtual int read() = 0;
    virtual int write() = 0;
};

template <IP T, int PORT>
class TCPServer : public TCPSocket<T, PORT> {
    int client_fd;
    struct sockaddr addr;
    socklen_t addr_len;

  protected:

  public:
    string send_str;
    string recv_str;
    TCPServer(T ip) : TCPSocket<T, PORT>(ip) {
        if (bind(this->socket_fd, (struct sockaddr *)&this->servaddr, sizeof(this->servaddr)) ==
            -1) {
            perror("Bind failed");
            throw runtime_error("Socket bind failed");
        }
        debug("Socket bound");
    }
    int accept() {
        int err;
        if ((client_fd = ::accept(this->socket_fd, &addr, &addr_len)) < 0) {
            debug("Error Accepting client");
        } else
            debug("Accepted client");

        return client_fd;
    }

    string getRequest() { return string(recv_str); }
    void setResponse(string response) { send_str = response; }

    int listen() {
        int err;
        if ((err = ::listen(this->socket_fd, 5)) < 0) {
            debug("Error Listening");
        } else {
            debug("Listening...");
        }
        return err;
    }

    int read() {
        int bytes;
        char temp[1024];
        if ((bytes = ::recv(client_fd, temp, 1024, 0)) < 0) {
            debug("Error reading from client");
        } else {
            recv_str = temp;
            debug("Read from client", bytes);
        }
        return bytes;
    }
    int write() {
        int err;
        if ((err = ::send(client_fd, send_str.data(), send_str.size(), 0)) < 0) {
            debug("Error writing to client");
        } else {
            debug("Wrote to client", err, "bytes");
        }
        return err;
    }
};

template <IP T, int PORT>
class TCPClient : TCPSocket<T, PORT> {
    conditional_t<is_same_v<IPv4, IPv4>, sockaddr_in, sockaddr_in6> addr;
    socklen_t addr_len;

  public:
    string buffer;

    TCPClient(T ip) : TCPSocket<T, PORT>(ip) {}

    int read() {
        int bytes;
        if ((bytes = ::recv(this->socket_fd, buffer.data(), buffer.size() + 1, 0)) < 0) {
            debug("Error reading from server");
        } else {
            debug("Read from server", bytes, "bytes");
        }
        return bytes;
    }

    int write() {
        int bytes;
        if ((bytes = ::send(this->socket_fd, buffer.data(), buffer.size() + 1, 0)) < 0) {
            debug("Error writing to server");
        } else {
            debug("Wrote to server", bytes, "bytes");
        }
        return bytes;
    }
};
} // namespace net
