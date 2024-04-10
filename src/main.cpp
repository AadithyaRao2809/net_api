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
        HTTPServer<IPv4, 8080> server(ip1);
        server.listen();
        server.setStatusCode(200);
        server.addHeader("Content-Type", "text/html");
        int n=0;
        while (1) {
            server.accept();
            server.setContent("<html><body><h1>Hello, World! "+to_string(n++)+"</h1></body></html>");
            server.read();


            server.write();
        }
    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    return 0;
}
