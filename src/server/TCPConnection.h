#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <string>
#include <sstream>

// The size of the buffer used to read commands. 1KB
#define BUFF_SIZE 1024

/**
 * @brief TCPConnection class represents a tcp connection.
 * It stores the file descriptor representing the connection in the kernel
 * and provides an interface for performing non blocking event driven io
 */
class TCPConnection {
private:
    int fd;
    ssize_t readPos;
    char buff[BUFF_SIZE];
    std::stringstream ss;
    // A variable to execute the command read.
public:
    TCPConnection();
    TCPConnection(int _fd);
    bool Read();
    int Write(std::string msg);
    void Close();
    std::string GetCommand();
};

#endif 