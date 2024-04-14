#include <string>
#ifndef TCP_F
#include "sock.cpp"
#define TCP_F

namespace net {
using namespace net;

template <typename T>
class Server;

template <IP T, int PORT>
class TCPSocket : public Socket<T, PORT> {

  public:
    TCPSocket(T ip) : Socket<T, PORT>(ip) { this->createSocket(); }

    int protocol() { return SOCK_STREAM; }
};

template <IP T, int PORT>
class TCPServer : public TCPSocket<T, PORT> {
    int client_fd;
    struct sockaddr addr;
    socklen_t addr_len;

    friend class Server<TCPServer<T, PORT>>;

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
        recv_str.resize(1024);
        int bytes;
        if ((bytes = ::recv(client_fd, recv_str.data(), 1024, 0)) < 0) {
            debug("Error reading from client");
        } else {
            recv_str.resize(bytes);
            debug("Read from client", bytes);
        }
        return bytes;
    }
    int write(string response) {
        int err;
        if ((err = ::send(client_fd, response.data(), response.size(), 0)) < 0) {
            debug("Error writing to client");
        } else {
            debug("Wrote to client", err, "bytes");
        }
        return err;
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
    int close() {
        int err;
        if ((err = ::close(client_fd)) < 0) {
            debug("Error closing client");
        } else {
            debug("Closed client");
        }
        return err;
    }
};

template <IP T, int PORT>
class TCPClient : public TCPSocket<T, PORT> {
    conditional_t<is_same_v<IPv4, IPv4>, sockaddr_in, sockaddr_in6> addr;
    socklen_t addr_len;

  public:
    string send_str;
    string recv_str;

    TCPClient(T ip) : TCPSocket<T, PORT>(ip) {}

    int connect() {
        int err;
        if ((err = ::connect(this->socket_fd, (struct sockaddr *)&this->servaddr,
                             sizeof(this->servaddr))) < 0) {
            debug("Error connecting to server");
        } else {
            debug("Connected to server");
        }
        return err;
    }

    int read() {
        int bytes;
        recv_str.resize(1024);
        if ((bytes = ::recv(this->socket_fd, recv_str.data(), recv_str.size() + 1, 0)) < 0) {
            debug("Error reading from server");
        } else {
            recv_str.resize(bytes);
            debug("Read from server", bytes, "bytes");
        }
        return bytes;
    }

    int write() {
        int bytes;
        if ((bytes = ::send(this->socket_fd, send_str.data(), send_str.size() + 1, 0)) < 0) {
            debug("Error writing to server");
        } else {
            debug("Wrote to server", bytes, "bytes");
        }
        return bytes;
    }
};


} // namespace net

#include "server.cpp"
#endif
