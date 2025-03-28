#ifndef TCPCONTEXT_H
#define TCPCONTEXT_H

#include <string>
#include <sstream>
#include <vector>

#include <commands/Command.h>

// The size of the buffer used to read commands. 1KB
#define BUFF_SIZE 1024

/**
 * @brief TCPContext class represents a tcp connection and all the associated 
 * actions to parse the reqest and return a reply. It stores the file descriptor
 * representing the connection in the kernel and provides an interface for 
 * performing non blocking event driven I/O.
 */
class TCPContext {
private:
    int fd;
    ssize_t readPos;
    char buff[BUFF_SIZE];
    std::stringstream ss;
    Command cmd;

    // A variable to execute the command read.
public:
    TCPContext();
    TCPContext(int _fd);
    ssize_t Read();
    ssize_t Write(std::string msg);
    void Close();
    std::string GetCommand();
    int GetReadPos();
};

#endif 