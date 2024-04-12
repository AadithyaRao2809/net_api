#ifndef HTTP_F
#define HTTP_F

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

#include "debug.cpp"
#include "tcp.cpp"

namespace net {
template <IP T, int PORT = 80>
class HTTPServer {

    TCPServer<T, PORT> server;

    string res_header;
    string res_body;
    // FIX: Remove std implementation
    std::unordered_map<string, string> res_headers;
    std::unordered_map<string, string> req_headers;
    std::unordered_map<string,void(*)()> routes;

  public:
    HTTPServer(T ip) : server(ip) {}
    HTTPServer &addHeader(string key, string value) {
        res_headers[key] = value;
        return *this;
    }

    int setStatusCode(int code) {
        switch (code) {
        case 200:
            res_headers["ver"] = "HTTP/1.0 200 OK\r\n";
            break;
        case 404:
            res_headers["ver"] = "HTTP/1.0 404 Not Found\r\n";
            break;
        case 503:
            res_headers["ver"] = "HTTP/1.0 500 Internal Server Error\r\n";
            break;
        default:
            res_headers["ver"] = "HTTP/1.0 200 OK\r\n";
        }
        return 0;
    }

    int setContent(string content) {
        res_headers["Content-Length"] = to_string(content.length());
        res_header = res_headers["ver"];

        for (auto &i : res_headers) {
            if (i.first == "ver")
                continue;
            res_header += i.first + ": " + i.second + "\r\n";
        }
        res_header += "\r\n";
        this->res_body = content;

        return 0;
    }

    int addRoute(string path, void (*callback)()) {
        return 0;
    }

std::unordered_map<std::string, std::string> parseHeader(const std::string& header) {
    std::unordered_map<std::string, std::string> headers;

    size_t pos = 0;
    // Find the end of the first line
    size_t endOfFirstLine = header.find("\r\n");
    if (endOfFirstLine == std::string::npos) {
        // Invalid header format, first line not found
        return headers;
    }

    // Extract the first line
    std::string firstLine = header.substr(0, endOfFirstLine);

    // Find the position of the first space to separate method and path
    size_t firstSpacePos = firstLine.find(' ');
    if (firstSpacePos == std::string::npos) {
        // Invalid header format, first line doesn't contain expected separators
        return headers;
    }

    // Extract method
    std::string method = firstLine.substr(0, firstSpacePos);

    // Find the position of the second space to separate path and HTTP version
    size_t secondSpacePos = firstLine.find(' ', firstSpacePos + 1);
    if (secondSpacePos == std::string::npos) {
        // Invalid header format, first line doesn't contain expected separators
        return headers;
    }

    // Extract path
    std::string path = firstLine.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);

    // Extract HTTP version
    std::string httpVersion = firstLine.substr(secondSpacePos + 1);

    // Store method, path, and HTTP version in the unordered_map
    headers["Method"] = method;
    headers["Path"] = path;
    headers["HTTPVersion"] = httpVersion;

    // Now parse the rest of the headers
    pos = endOfFirstLine + 2; // Move past the first line (\r\n)

    while (pos < header.size()) {
        size_t colonPos = header.find(':', pos);
        if (colonPos == std::string::npos) {
            break;
        }

        std::string key = header.substr(pos, colonPos - pos);

        size_t endPos = header.find("\r\n", colonPos);
        if (endPos == std::string::npos) {
            break;
        }

        std::string value = header.substr(colonPos + 2, endPos - colonPos - 2);

        headers[key] = value;

        pos = endPos + 2;
    }
    return headers;
}
    unordered_map<string, string> parseHeader() {
        return parseHeader(server.recv_str);
    }

    int listen() { return server.listen(); }
    int accept() { return server.accept(); }

    string read() {
        server.read();
        return server.recv_str;
    }

    int write() {
        server.send_str = res_header + res_body;
        int err = server.write();
        return err;
    }
    int close() { return server.close(); }

    int startServer(const string root_dir = ".") {
        listen();
        while (1) {
            accept();
            read();
            auto headers = parseHeader();
            for (auto &i : headers) {
                debug(i.first, i.second);
            }
            if(headers["Path"] == "/") {
                headers["Path"] = "/index.html";
            }
            string path = root_dir + headers["Path"];
            debug("\t Path:",path);
            int fd = open(path.c_str(), O_RDONLY);
            if (fd < 0) {
                debug("File not found");
                setStatusCode(404);
                setContent("404 Not Found");
            } else {
                setStatusCode(200);
                char buffer[1024];
                int n;
                string content;
                while ((n = ::read(fd, buffer, 1024)) > 0) {
                    content += string(buffer, n);
                }
                setContent(content);
            }
            write();
        }
        return 0;
    }

    ~HTTPServer() { close(); }
};

template <IP T, int PORT=80>
class HTTPClient {
    TCPClient<T,PORT> client;
    string req_header;
    string res_header;
    string res_body;

};

} // namespace net
#endif
