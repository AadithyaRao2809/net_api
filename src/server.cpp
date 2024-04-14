#ifndef SERVER_F
#include "sock.cpp"

#define SERVER_F

namespace net {
template <typename T>
class Server {
};


template <IP T, int PORT>
class Server<TCPServer<T,PORT>> {
    TCPServer<T,PORT> server;

  public:
    Server(auto ip) : server(T(ip)) {
    }
    void start() {
        server.listen();
        while (1) {
            server.accept();
            server.read();
            // send_str is private 
            server.send_str = "Hello from server";
            server.write();
            server.close();

        }
    }
};
} // namespace net

#endif
