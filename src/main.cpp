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




int main(int argc, char **argv) {
   
   try{
    IPv4 ip("localhost");
    startServer<HTTPServer<IPv4, 8080>, IPv4>(ip);
   }
   catch(const std::exception& e){
       cout << e.what() << endl;
   }
    return 0;
}
