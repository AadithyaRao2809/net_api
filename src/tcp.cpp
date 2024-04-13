#ifndef TCP_F
#include "sock.cpp"
#define TCP_F


using namespace net;
template <IP T, int PORT>
class TCPSocket :public Socket<T,PORT>{

    public:
    TCPSocket(T ip) : Socket<T, PORT>(ip) {this->createSocket();}

    int protocol() {
        return SOCK_STREAM;
    }


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
    string buffer;

    TCPClient(T ip) : Socket<T, PORT>(ip) {}

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


#endif
