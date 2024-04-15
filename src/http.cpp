#ifndef HTTP_F
#define HTTP_F

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "debug.cpp"
#include "map.cpp"
#include "tcp.cpp"

namespace net {

struct Header {
    string key;
    string value;
    Header(string key, string value) : key(key), value(value) {}
    Header(string header) {
        size_t pos = header.find(":");
        key = header.substr(0, pos);
        value = header.substr(pos + 1);
    }
};


template <typename T>
class Server;

template <typename T>
concept HeaderType = is_same_v<T, Header>;

template <IP T, int PORT = 80>
class HTTPServer {

    TCPServer<T, PORT> server;

    string res_header;
    string res_body;
    string root_dir;
    UnorderedMap<string, string> res_headers;
    UnorderedMap<string, string> req_headers;

    friend class Server<HTTPServer<T, PORT>>;

  public:
    HTTPServer(T ip) : server(ip), root_dir("./"){}
    void addHeader(string key, string value) { res_headers.insert(key, value); }


    template <HeaderType... Args>
    void addHeaders(const Args &...args) {
        (addHeader(args.key, args.value), ...);
    }

    // set the correct header according to the status code
    int setStatusCode(int code) {
        switch (code) {
        case 200:
            res_headers.insert("ver", "HTTP/1.0 200 OK\r\n");
            break;
        case 404:
            res_headers.insert("ver", "HTTP/1.0 404 Not Found\r\n");
            break;
        case 503:
            res_headers.insert("ver", "HTTP/1.0 503 Internal Server Error\r\n");
            break;
        default:
            res_headers.insert("ver", "HTTP/1.0 200 OK\r\n");
        }
        return 0;
    }
    void setRootDir(string dir) { root_dir = dir; }

    // calculate the content length and set the content type
    int setContent(string content) {
        res_headers.insert("Content-Type", "text/html");
        res_headers.insert("Content-Length", to_string(content.size()));
        res_header = res_headers["ver"];

        Node<string, string> *temp = res_headers.getElements();
        while (temp != nullptr) {
            if (temp->key == "ver") {
                temp = temp->next;
                continue;
            }
            res_header += temp->key + ": " + temp->value + "\r\n";
            temp = temp->next;
        }
        res_header += "\r\n";
        this->res_body = content;

        return 0;
    }

    // parse the header from request and store it in an unordered_map
    UnorderedMap<std::string, std::string> parseHeader(const std::string &header) {
        UnorderedMap<std::string, std::string> headers;

        size_t pos = 0;
        size_t endOfFirstLine = header.find("\r\n");
        if (endOfFirstLine == std::string::npos) {
            return headers;
        }

        std::string firstLine = header.substr(0, endOfFirstLine);

        // Find the position of the first space to separate method and path
        size_t firstSpacePos = firstLine.find(' ');
        if (firstSpacePos == std::string::npos) {
            return headers;
        }

        // Extract method
        std::string method = firstLine.substr(0, firstSpacePos);

        size_t secondSpacePos = firstLine.find(' ', firstSpacePos + 1);
        if (secondSpacePos == std::string::npos) {
            return headers;
        }

        // Extract path
        std::string path = firstLine.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);

        // Extract HTTP version
        std::string httpVersion = firstLine.substr(secondSpacePos + 1);

        // Store method, path, and HTTP version in the unordered_map
        headers.insert("Method", method);
        headers.insert("Path", path);
        headers.insert("HTTPVersion", httpVersion);

        pos = endOfFirstLine + 2;

        // parse the rest of the header
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

            headers.insert(key, value);

            pos = endPos + 2;
        }
        return headers;
    }

    UnorderedMap<string, string> parseHeader() { return parseHeader(server.recv_str); }

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

    // create a prebuilt server

    ~HTTPServer() { close(); }
};

template <IP T, int PORT>
class Server<HTTPServer<T, PORT>> {
    HTTPServer<T, PORT> server;

  public:
    Server(T ip) : server(ip) {}
    void setRootDir(string dir) { server.setRootDir(dir); }

    void start() {

        server.listen();
        while (1) {
            int client_fd = server.accept();

            server.read();

            auto headers = server.parseHeader();

            if (headers["Path"] == "/") { // sets base path as index.html
                headers.insert("Path", "/index.html");
            }
            string path = server.root_dir + headers["Path"];
            debug("\t Path:", path);
            int fd = open(path.c_str(), O_RDONLY);
            if (fd < 0) {
                // file not found
                debug("File not found");
                server.setStatusCode(404);
                server.setContent("<h1>404 Not Found</h1>");
            } else {
                // read file into string
                server.setStatusCode(200);
                char buffer[1024];
                int n;
                string content;
                while ((n = ::read(fd, buffer, 1024)) > 0) {
                    content += string(buffer, n);
                }
                server.addHeaders(Header("Connection", "close"), Header("Server", "HTTPServer"), Header("Content-Type", "text/html"));
                server.setContent(content);
            }
            server.write();
            server.close();
        }
    }
};

} // namespace net
#endif
