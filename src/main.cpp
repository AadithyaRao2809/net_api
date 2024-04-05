#include <arpa/inet.h>
#include <cstdint>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
// #define DEBUG
template <typename... T>
void debug_log(string file, int line, T... args) {
#ifdef DEBUG
    clog << "[" << file << ":" << line << "] ";
    ((clog << args << " "), ...);
    clog << endl;

#endif
    return;
}

#define debug(...) debug_log(__FILE__, __LINE__, __VA_ARGS__)

class IPv4 {
  public:
    uint32_t address;

    IPv4(uint32_t address) : address(htonl(address)) {}

    IPv4(const string &address) {
        if (address == "localhost") {
            this->address = htonl(INADDR_LOOPBACK);
            return;
        }
        struct in_addr addr;
        if (inet_pton(AF_INET, address.c_str(), &addr) != 1) {
            throw invalid_argument("Invalid IPv4 address");
        }
        this->address = addr.s_addr;
    }

    operator uint32_t() const { return ntohl(address); }
    operator string() const {
        struct in_addr addr;
        addr.s_addr = address;
        return inet_ntoa(addr);
    }
};
union IPV6_addr {
    uint8_t bytes[16];
    uint16_t word[8];
    uint32_t dword[4];
};

class IPv6 {
  public:
    IPV6_addr address;
    IPv6(string ipstr) {}
    template <typename... T>
    IPv6(T... args) {
        const int temp[] = {args...};
        int i = 0;
        if (sizeof...(args) == 16) {
            for (auto elem : temp)
                address.bytes[i++] = elem;
        } else if (sizeof...(args) == 8) {
            for (auto elem : temp)
                address.word[i++] = ntohs(elem);
        } else if (sizeof...(args) == 4) {
            for (auto elem : temp)
                address.dword[i++] = ntohl(elem);
        } else
            throw invalid_argument("Invalid IPv6 address");
    }

    operator string() const {
        char str[INET6_ADDRSTRLEN];
        if (inet_ntop(AF_INET6, &address, str, INET6_ADDRSTRLEN)) {
            return str;
        }
        return "";
    }
};

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
        } else
            throw invalid_argument("Invalid IP type");

        // Binding socket to the IP and port
    }

    virtual int read() = 0;
    virtual int write() = 0;
};

template <IP T, int PORT>
class TCPServer : public TCPSocket<T, PORT> {
    int client_fd;
    struct sockaddr addr;
    socklen_t addr_len;

  public:
    char buffer[1024];

    TCPServer(T ip) : TCPSocket<T, PORT>(ip) {
        memset(buffer, 0, 1024);
        if (bind(this->socket_fd, (struct sockaddr *)&this->servaddr, sizeof(this->servaddr)) == -1) {
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

    string getBuffer() { return string(buffer); }

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
        if ((bytes = ::recv(client_fd, buffer, 1024, 0)) < 0) {
            debug("Error reading from client");
        } else {
            debug("Read from client", bytes, "bytes");
            buffer[bytes] = '\0';
        }
        return bytes;
    }
    int write() {
        int err;
        if ((err = ::send(client_fd, buffer, 1024, 0)) < 0) {
            debug("Error writing to client");
        } else {
            debug("Wrote to client");
        }
        return err;
    }
};

template <IP T, int PORT>
class TCPClient : TCPSocket<T, PORT> {
    conditional_t<is_same_v<IPv4, IPv4>, sockaddr_in, sockaddr_in6> addr;
    socklen_t addr_len;

  public:
    char buffer[1024];

    TCPClient(T ip) : TCPSocket<T, PORT>(ip) { memset(buffer, 0, 1024); }

    int connect() {
        int err;
        if constexpr (is_same_v<T, IPv4>) {
            addr = static_cast<sockaddr_in>(this->servaddr);
            addr_len = sizeof(this->servaddr);
            if ((err = ::connect(this->socket_fd, (struct sockaddr *)&addr, addr_len)) < 0) {
                debug("Error connecting to server");
            } else {
                debug("Connected to server");
            }
        } else {
            // addr = static_cast<sockaddr_in6>(this->servaddr);
            addr_len = sizeof(this->servaddr);
        }
        return err;
    }

    int read() {
        int bytes;
        if ((bytes = ::recv(this->socket_fd, buffer, 1024, 0)) < 0) {
            debug("Error reading from server");
        } else {
            debug("Read from server", bytes, "bytes");
            buffer[bytes] = '\0';
        }
        return bytes;
    }

    int write() {
        int bytes;
        if ((bytes = ::send(this->socket_fd, buffer, 1024, 0)) < 0) {
            debug("Error writing to server");
        } else {
            debug("Wrote to server", bytes, "bytes");
        }
        return bytes;
    }
};

int main() {

    IPv6 ip2(0x0, 0x0, 0x0, 0x0);
    IPv4 ip1("localhost");
    try {
        // TCPServer<IPv6, 8080> server(ip2);
        TCPClient<IPv4, 8080> server(ip1);
        server.connect();
        strcpy(server.buffer, "Hello from client\n");
        cout << server.buffer << endl;
        server.write();
        server.read();
        cout << server.buffer << endl;
    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    return 0;
}
