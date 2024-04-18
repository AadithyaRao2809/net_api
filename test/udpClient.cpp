#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include "../src/udp.cpp"

using namespace std;
using namespace net;

    void startClient(){
       
    }
int main(int argc, char **argv) {
    IPv4 ip;
    if (argc > 1) {
        ip = IPv4(argv[1]);
    } else {
    IPv4 ip("localhost");
    }

    UDPClient<IPv4, 8080> client(ip);
     while(1){
            string response;
            cout << "Enter the message to send: ";
            getline(cin, response);
            client.setRequest(response);
            client.write();
            client.read();
            cout << "Server: " << client.getResponse() << endl;
            
        }
    
    cout << "Server: " << client.getResponse() << endl;
}
