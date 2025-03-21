#include <spdlog/spdlog.h>

#include "server/TCPServer.h"
#include "common/Exceptions.h"

int main(int argc, char **argv)
{
    TCPServer server;
    try {
        server.SetupServer();
    } catch(TCPError& e) {
        spdlog::error("Error occurred while setuping up the server: {}", e.what());
    }
    server.EventLoop();
    // {
    //     char buffer[1024] = {0};
    //     ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    //     if (bytes_read > 0)
    //     {
    //         buffer[bytes_read] = '\0';
    //         std::cout << "Received message: " << buffer << "\n";
    //     }
    //     else if (bytes_read < 0)
    //     {
    //         std::cerr << "Error while reading message\n";
    //         close(client_fd);
    //         return 1;
    //     } else {
    //         std::cout << "Nothing to read\n";
    //         close(client_fd);
    //         break;
    //     }

    //     std::string pong_response = "+PONG\r\n";
    //     ssize_t response = send(client_fd, pong_response.c_str(), pong_response.size(), 0);
    //     if (response != pong_response.size())
    //     {
    //         std::cerr << "Error while sending a response\n";
    //         close(client_fd);
    //         return 1;
    //     }
    //     else
    //     {
    //         std::cout << "Successfully sent the response\n";
    //     }
    // }
    // close(server_fd);

    return 0;
}
