#include <cstdint>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

class IPv4 {
  uint32_t address;

public:
  IPv4(uint32_t address) : address(address) {}

  IPv4(const string &address) {
    struct in_addr addr;
    if (inet_pton(AF_INET, address.c_str(), &addr) != 1) {
      throw invalid_argument("Invalid IPv4 address");
    }
    this->address = addr.s_addr;
  }

  operator uint32_t() const { return address; }
  operator string() const {
    struct in_addr addr;
    addr.s_addr = address;
    return inet_ntoa(addr);
  }



};

class IPv6 {};

int main() {
  IPv4 ip1(0x7F000001);
  cout << string(ip1) << endl;

  IPv4 ip2("192.168.0.1");
  cout << (string)ip2 << endl;


}
