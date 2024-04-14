#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "http.cpp"
#include "debug.cpp"
#include "udp.cpp"



using namespace std;
using namespace net;

int main(int argc, char **argv) {
/*     #ifdef SERVER
    IPv4 ip1;
    try {
        // TCPServer<IPv6, 8080> server(ip2);
       UDPServer<IPv4, 8080> server(ip1);
        server.startServer();

    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    #endif
    #ifndef SERVER
    string ip_str;
    cout << "Enter the IP address of the server: " << endl;
    cin >> ip_str;
    debug("IP: " + ip_str);
    IPv4 ip2(ip_str);
    try{
        UDPClient<IPv4, 8080> client(ip2);
        client.startClient();
    }
    catch(const exception &e){
        cout << e.what() << endl;
    }
    #endif
    return 0;
 */
    IPv4 ip1;
    HTTPServer<IPv4, 8080> server(ip1);
    server.startServer("./test");
    return 0;
}
