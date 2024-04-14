#ifndef SERVER_F
#include "sock.cpp"
#include "tcp.cpp"

#define SERVER_F

namespace net {
template <typename T, int PORT>
class Server {
    TCPServer<T, PORT> server;

  public:
    Server() : server(T()) { debug(server.client_fd); }
    Server(T ip) : server(ip) { debug(server.client_fd); }
    void start() {
        server.listen();
        while (1) {
            int client_fd = server.accept();
            string request = server.getRequest();
            debug("Request: ", request);
            server.setResponse(
                "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello, World!</h1>");
            debug("Response: ", server.send_str);
        }
    }
};
} // namespace net

#endif
