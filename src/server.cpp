#ifndef SERVER_F
#include "sock.cpp"

#define SERVER_F

namespace net {
template <typename T>
class Server {
    T server;

  public:
    Server(auto ip) : server(T(ip)) {
    }
    void start() {
        server.listen();
        while (1) {
            server.accept();
            server.read();
            server.write(server.getRequest());
            server.close();

            debug("Response: ", server.getRequest());
        }
    }
};
} // namespace net

#endif
