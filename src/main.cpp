#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "tcp.cpp"



using namespace std;
using namespace net;

int main(int argc, char **argv) {
    IPv4 ip("localhost");
    Server<TCPServer<IPv4, 8080>> server(ip);
}
