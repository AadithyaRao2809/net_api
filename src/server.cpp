#ifndef SERVER_F
#include "sock.cpp"
#include "debug.cpp"

#define SERVER_F

namespace net {

// base implementation of a server
template <typename T>
class Server {
    // if it calls this class, it is an error
    Server() {
        debug("Server class called");
        throw runtime_error("Server class called");
    }

};

} // namespace net

#endif
