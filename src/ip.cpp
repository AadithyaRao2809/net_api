#ifndef IP_F

#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define IP_F

namespace net {

template <typename T>
concept Integral = std::is_integral_v<T>;

class IPv4 {
  public:
    uint32_t address;
    IPv4(){
        this->address = htonl(INADDR_ANY);
        return;
    }
    IPv4(uint32_t address) : address(htonl(address)) {}

    IPv4(const std::string &address) {
        if (address == "localhost") {
            this->address = htonl(INADDR_LOOPBACK);
            return;
        }
        struct in_addr addr;
        if (inet_pton(AF_INET, address.c_str(), &addr) != 1) {
            throw std::invalid_argument("Invalid IPv4 address");
        }
        this->address = addr.s_addr;
    }

    operator uint32_t() const { return ntohl(address); }
    operator std::string() const {
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


    IPv6(const std::string &address) {
        if (address == "localhost") {
            for (int i = 0; i < 16; i++)
                this->address.bytes[i] = 0;
            this->address.word[5] = 0;
            return;
        }
        struct in6_addr addr;
        if (inet_pton(AF_INET6, address.c_str(), &addr) != 1) {
            throw std::invalid_argument("Invalid IPv6 address");
        }
        for (int i = 0; i < 16; i++)
            this->address.bytes[i] = addr.s6_addr[i];
    }
    template <Integral... T> requires(sizeof...(T) == 16 || sizeof...(T) == 8 || sizeof...(T) == 4)
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
            throw std::invalid_argument("Invalid IPv6 address");
    }
    operator std::string() const {
        char str[INET6_ADDRSTRLEN];
        if (inet_ntop(AF_INET6, &address, str, INET6_ADDRSTRLEN)) {
            return str;
        }
        return "";
    }
};

} // namespace net
#endif
