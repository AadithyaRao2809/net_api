#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

void debug(string str) {
#if DEBUG == 1
    cout << str << endl;

#endif
    return;
}

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
    uint16_t hword[8];
    uint32_t word[4];
};

class IPv6 {

    IPV6_addr address;

  public:
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
                address.hword[i++] = ntohs(elem);
        } else if (sizeof...(args) == 4) {
            for (auto elem : temp)
                address.word[i++] = ntohl(elem);
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
    struct sockaddr_in servaddr, clientaddr;
    int server_fd = -1, client_fd = -1;
    char buffer[1024];

  public:
    TCPSocket(T ip) : ip_addr(ip){
        servaddr.sin_port = htons(PORT);
        debug("Port set "+to_string(PORT));
        if (std::is_same_v<T, IPv4>) /* Check if IP is v4 */ {
            servaddr.sin_family = AF_INET; //AF_INET is for IPv4
            servaddr.sin_addr.s_addr = ip_addr.address;
            if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) /* SOCK_STREAM is for TCP */{
                throw runtime_error("Socket creation failed");
            }
            debug("Socket created");
            if(bind(server_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
                throw runtime_error("Socket bind failed");
            }
            debug("Socket bound");
        }
    }
    int accept() {
        int len = sizeof(clientaddr);
        client_fd = ::accept(server_fd, (struct sockaddr *)&clientaddr, (socklen_t *)&len);
        if (client_fd < 0) {
            throw runtime_error("Server accept failed");
        }
        debug("Server accepted connection");
        return client_fd;
    }

    int listen() {
        if (::listen(server_fd, 5) < 0) {
            throw runtime_error("Server listen failed");
        }
        debug("Server listening");

        accept();

        read(client_fd, buffer, sizeof(buffer));
        cout << buffer << endl;

        return 0;
    }
};

int main() {

    IPv4 ip2("localhost");
    debug(string(ip2));
    try {
        TCPSocket<IPv4, 8080> server(ip2);
        server.listen();
    } catch (const exception &e) {
        cout << e.what() << endl;
    }
}
