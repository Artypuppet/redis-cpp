#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <TCPConnection.h>
#include <unordered_map>

#define MAX_EVENTS 10000
#define MAX_TIMEOUT 100 // The number of milliseconds a call to epoll_wait should block.

/**
 * @brief TCPServer class provides a high level abstraction for actions related
 * to a tcp connection. It allows us to create TCP server and then accept 
 * multiple connections through this server. It allows for non blocking client
 * accept connections and reads using epoll. It creates an epoll instance to monitor
 * tcp connections ready for I/O.
 */
class TCPServer {
private:
    struct epoll_event ev, events[MAX_EVENTS];
    int serverfd, epollfd;
    std::unordered_map<int, TCPConnection> connections;

    void addNewConnection();
    void handleEvent(int fd);
public:
    TCPServer();
    void SetupServer();
    int EventLoop();
};