#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <spdlog/spdlog.h>
#include "server/TCPConnection.h"
#include "server/TCPServer.h"
#include "common/Exceptions.h"
#include "commands/Command.h"

using std::string;

TCPServer::TCPServer(): serverfd(0), epollfd(0) {}

void TCPServer::SetupServer() {
    // Create a non blocking server
    serverfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (serverfd < 0) throw TCPError(-1);

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) throw TCPError(serverfd);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379); // Setup on port 6379

    if (bind(serverfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) throw TCPError(serverfd);
    if (listen(serverfd, MAX_EVENTS) != 0) throw TCPError(serverfd);

    // Create the epoll instance
    epollfd = epoll_create1(0);
    if (epollfd == -1) throw TCPError(epollfd);

    // Add the serverfd to the epoll interest list to be able to listen
    // for incoming connections.
    ev.events = EPOLLIN;
    ev.data.fd = serverfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverfd, &ev) == -1) throw TCPError(epollfd);
}

// TCPServer::EventLoop is the main event loop that listens for events on fds
// registered with the epoll instance. It adds a new connection when an event
// is triggered on the socket's fd otherwise handles the incoming write events
// on a current TCP Connection.
int TCPServer::EventLoop() {
    int i, nfds = 0;
    while(true) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, MAX_TIMEOUT);
        spdlog::debug("Epoll wait returned {} events", nfds);
        if (nfds < 0) throw TCPError(epollfd);
        else if (nfds == 0) std::this_thread::sleep_for(std::chrono::milliseconds(400));

        // Handle the events
        for(i = 0; i < nfds; i++) {
            if(events[i].data.fd == serverfd) {
                try {
                addNewConnection();
                } catch (TCPError& e) {
                    spdlog::error("Error while accepting a new TCP connection: {}", e.what());
                }
            } else {
                handleEvent(i);
            }
        }

        // Handle the commands.
        while(!commands.empty()) {
            Command cmd = std::move(commands.front());
            commands.pop();
            contexts[cmd.connfd].Write(cmd.Run());
        }
    }
}

// TCPServer::addNewConnection accepts a new TCP connection, registers it with
// the epoll instance and adds it to the map of TCP connections.
void TCPServer::addNewConnection() {
    // Accept the connection
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int clientfd = accept4(serverfd, (struct sockaddr*) &clientAddr, (socklen_t*) &clientAddrLen, SOCK_NONBLOCK);
    if(clientfd == -1) throw TCPError(-1);

    // Make the connnection non blocking
    int flags = fcntl(clientfd, F_GETFL, 0);
    if (flags == -1) throw TCPError(clientfd);
    
    // Instead of using TCPError create a new exception or add an additional parameter to
    // set the msg of what exactly went wrong. In this case O_NONBLOCK wasn't set.
    if (!(flags & O_NONBLOCK)) throw TCPError(clientfd, "unable to make the file descriptor non-blocking");

    // Register the fd with the epoll instance
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = clientfd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev) == -1) throw TCPError(clientfd);

    // Finally add the conection to the map
    contexts.emplace(clientfd, clientfd);
}

// TCPServer::handleEvent handles the read edge trigger events on a TCP connection
// or a connection hangup from the peer.
void TCPServer::handleEvent(int i) {
    int connfd = events[i].data.fd;
    TCPContext& ctx = contexts[connfd];
    // Handle connection termination and return immediately
    if (events[i].events & (EPOLLHUP | EPOLLHUP)) {
        closeConn(ctx, connfd);
        return;
    }
    if(events[i].events & EPOLLIN) {
        // Read until EOF or if EWOULDBLOCK or EAGAIN is returned
        ssize_t n = 0;
        do {
            try {
                n = ctx.Read();
                spdlog::debug("Reading {} bytes from connection {}", n, connfd);
            } catch(TCPError& e) {
                spdlog::error("Error while reading from socket {}: {}", connfd, e.what());
            }
        } while(n > 0);
        if(ctx.GetReadPos() > 0) {
            commands.emplace(connfd, contexts[connfd].GetCommand());
        } else if (ctx.GetReadPos() == 0) {
            // Read until EOF, close the connection
            closeConn(ctx, connfd);
        }
    }
}


void TCPServer::closeConn(TCPContext& ctx, int connfd) {
    try {
        // close() sys call will also remove the fd from the epoll instance.
        spdlog::debug("Got close event on connection {}: closing...", connfd);
        ctx.Close();
    } catch(TCPError& e) {
        spdlog::error("Error while closing socket {}: {}", connfd, e.what());
    }
    contexts.erase(connfd);
}