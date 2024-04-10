#ifndef HTTP_F
#define HTTP_F

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

#include "debug.cpp"
#include "tcp.cpp"

namespace net {
template <IP T, int PORT = 80>
class HTTPServer {

    TCPServer<T, PORT> server;
    string header;
    string body;
    // FIX: Remove std implementation
    std::unordered_map<string, string> headers;

  public:
    HTTPServer(T ip) : server(ip) {}
    HTTPServer &addHeader(string key, string value) {
        headers[key] = value;
        return *this;
    }

    int setStatusCode(int code) {
        switch (code) {
        case 200:
            headers["ver"] = "HTTP/1.0 200 OK\r\n";
            break;
        case 404:
            headers["ver"] = "HTTP/1.0 404 Not Found\r\n";
            break;
        case 503:
            headers["ver"] = "HTTP/1.0 500 Internal Server Error\r\n";
            break;
        default:
            headers["ver"] = "HTTP/1.0 200 OK\r\n";
        }
        return 0;
    }

    int setContent(string content) {
        headers["Content-Length"] = to_string(content.length());
        header = headers["ver"];

        for (auto &i : headers) {
            if (i.first == "ver")
                continue;
            header += i.first + ": " + i.second + "\r\n";
        }
        header += "\r\n";
        this->body = content;

        return 0;
    }

    int listen() { return server.listen(); }
    int accept() { return server.accept(); }

    string read() {
        server.read();
        return server.recv_str;
    }

    int write() {
        server.send_str = header + body;
        int err = server.write();
        return err;
    }
    int close() { return server.close(); }

    // TODO: Implement this
    int startServer(const string root_dir = "./") { return 0; }
    ~HTTPServer() { close(); }
};

class HTTPClient {

};
} // namespace net
#endif
