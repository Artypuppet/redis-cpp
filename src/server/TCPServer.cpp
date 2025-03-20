#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <spdlog/spdlog.h>
#include <TCPConnection.h>
#include <TCPServer.h>
#include <common/Exceptions.h>

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
    server_addr.sin_port = htons(6379);

    if (bind(serverfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) throw TCPError(serverfd);
    if (listen(serverfd, MAX_EVENTS) != 0) throw TCPError(serverfd);

    // Create the epoll instance
    epollfd = epoll_create1(0);
    if (epollfd == -1) throw TCPError(epollfd);

    // Add the serverfd to the epoll interest list to be able to listen
    // for incoming connections.
    ev.events = EPOLLIN;
    ev.data.fd = serverfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverfd, &ev) != -1) throw TCPError(epollfd);
}

int TCPServer::EventLoop() {
    int i, nfds = 0;
    while(true) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, MAX_TIMEOUT);
        spdlog::debug("Epoll wait returned {} events", nfds);
        if (nfds < 0) throw TCPError(epollfd);
        // No events, put the thread to sleep
        if (!nfds) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIMEOUT));
            continue;
        }

        // Handle the events
        for(i = 0; i < nfds; i++) {
            if(events[i].data.fd == serverfd) {
                try {
                addNewConnection();
                } catch (TCPError& e) {
                    spdlog::error("Error while accepting a new TCP connection: {}", e.what());
                }
            } else {
                handleEvent(events[i].data.fd);
            }
        }
    }
}

void TCPServer::addNewConnection() {
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int clientfd = accept(serverfd, (sockaddr*) &clientAddr, (socklen_t*) clientAddrLen);
    if(clientfd == -1) throw TCPError(-1);
}