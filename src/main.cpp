#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

using namespace std;

class IPv4 {
    uint32_t address;

  public:
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
typedef union IPV6_addr {
    uint8_t bytes[16];
    uint16_t hword[8];
    uint32_t word[4];
} IPV6_addr;

template <typename... T>
class IPv6 {

    IPV6_addr address;

  public:
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
class TCPSocket {};
int main() {
    IPv4 ip1(0x7F000001);
    cout << string(ip1) << endl;

    IPv4 ip2("localhost");
    cout << std::hex << ip2 << endl;

    IPv6 ip3(0xfc00, 0xf853, 0xccd, 0xe793, 0x0, 0x0, 0x0, 0x0001);
    cout << string(ip3) << endl;
}
