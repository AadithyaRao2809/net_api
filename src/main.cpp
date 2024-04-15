#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "tcp.cpp"
#include "udp.cpp"
#include "http.cpp"



using namespace std;
using namespace net;

auto startServer = []<typename T>(auto ip) {
    Server<T> server(ip);
    server.start();
};
int main(int argc, char **argv) {
    IPv4 ip("localhost");
    startServer<HTTPServer<IPv4,8080>>(ip);


}
