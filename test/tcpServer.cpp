#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "../src/tcp.cpp"

using namespace std;
using namespace net;

int main(int argc, char **argv) {
    IPv4 ip;
    TCPServer<IPv4, 8080> server(ip);
    server.listen();
    while (1) {
        server.accept();
        server.read();
        server.send_str = "Hello from server";
        server.write();
        cout << "Client: " << server.recv_str << endl;
        server.close();
    }
}
