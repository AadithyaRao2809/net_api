#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "../src/udp.cpp"

using namespace std;
using namespace net;

int main(int argc, char **argv) {
    IPv4 ip;
    UDPServer<IPv4, 8080> server(ip);
    while (1) {
        server.read();
        server.setResponse("Hello from server");
        server.write();
        cout << "Client: " << server.getRequest() << endl;
    }
}
