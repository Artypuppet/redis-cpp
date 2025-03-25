#include <string>
#include <unistd.h>

#include "TCPConnection.h"
#include "common/Exceptions.h"

using std::string;

TCPConnection::TCPConnection() : fd(-1), readPos(0) {};

TCPConnection::TCPConnection(int _fd) : fd(_fd), readPos(0) {};

bool TCPConnection::Read() {
    ssize_t n = read(fd, buff, BUFF_SIZE);
    if (n < 0) {
        if (errno & (EAGAIN | EWOULDBLOCK)) return false;
        else throw TCPError(fd);
    } else if (n == 0) {
        readPos = 0;
        return false;
    } 
    ss << buff;
    readPos += n;
    return true;
}

int TCPConnection::Write(string msg) {
    ssize_t n = write(fd, msg.c_str(), msg.size());
    if (n < 0) throw TCPError(fd);
    return n;
}

void TCPConnection::Close() {
    int result = close(fd);
    if (result < 0) throw TCPError(fd);
}

string TCPConnection::GetCommand() {
    string cmd = ss.str();
    ss.str("");
    ss.clear();
    return cmd;
}