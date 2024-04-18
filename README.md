# net_api
A Networking API in C++

## Usage

### HTTP 
`startServer<HTTPServer<IP PROTOCOL, PORT>, PORT>(ip);`

### UDP

Server : 
`UDPServer<IP PROTOCOL, PORT> server(ip);`
Client : 
`UDPClient<IP PROTOCOL, PORT> client(ip);`

### TCP

`TCPServer<IP PROTOCOL, PORT> server(ip);`
Client : 
`TCPClient<IP PROTOCOL, PORT> client(ip);`


## A unified interface

There is also functionality to create the servers through the use of the startServer function, which acts as a common interface to call different server classes.
