#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include "../src/tcp.cpp"

using namespace std;
using namespace net;

int main(int argc, char **argv) {
    IPv4 ip;
    if (argc > 1) {
        ip = IPv4(argv[1]);
    } else {
    IPv4 ip("localhost");
    }

    TCPClient<IPv4, 8080> client(ip);
    client.connect();
    client.send_str = "Hello from client";
    client.write();
    client.read();
    cout << "Server: " << client.recv_str << endl;
}
