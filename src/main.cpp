#include <arpa/inet.h>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "http.cpp"
#include "debug.cpp"

using namespace std;
using namespace net;




int main() {

    IPv4 ip1("localhost");
    try {
        // TCPServer<IPv6, 8080> server(ip2);
        HTTPServer<IPv4> server(ip1);
        server.startServer("./test");

    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    return 0;
}
