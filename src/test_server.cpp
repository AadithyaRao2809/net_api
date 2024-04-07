#include "main.cpp"

int main() {
    TCPServer<IPv4, 8080> server(IPv4("localhost"));
    server.listen();
    server.accept();
    server.read();
    server.write();

}
