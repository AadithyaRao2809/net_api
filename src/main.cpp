#include <arpa/inet.h>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "http.cpp"
#include "debug.cpp"
#include "udp.cpp"



using namespace std;
using namespace net;

int main() {
    #ifdef SERVER
    IPv4 ip1;
    try {
        // TCPServer<IPv6, 8080> server(ip2);
       UDPServer<IPv4, 8080> server(ip1);
    

    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    #endif
    #ifndef SERVER
    string ip_str;
    cin >> ip_str;
    debug("IP: " + ip_str);
    IPv4 ip2(ip_str);
    try{
        UDPClient<IPv4, 8080> client(ip2);
        client.write("Hello from client");
    }
    catch(const exception &e){
        cout << e.what() << endl;
    }
    #endif
    return 0;

}
