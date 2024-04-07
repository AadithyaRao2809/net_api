#include "main.cpp"

int main() {
    TCPClient<IPv4, 8080> client(IPv4("localhost"));
    client.connect();
    strcpy(client.buffer, "Hello from client\n");
    client.write();
    client.read();
    cout << client.buffer << endl;
}
