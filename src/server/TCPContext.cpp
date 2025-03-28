#include <string>
#include <unistd.h>

#include "TCPContext.h"
#include "common/Exceptions.h"

using std::string;

TCPContext::TCPContext() : fd(-1), readPos(0) {};

TCPContext::TCPContext(int _fd) : fd(_fd), readPos(0) {};

ssize_t TCPContext::Read() {
    ssize_t n = read(fd, buff, BUFF_SIZE);
    if (n < 0) {
        if (errno & (EAGAIN | EWOULDBLOCK)) return 0;
        else throw TCPError(fd);
    } else if (n == 0) {
        readPos = 0;
    } else {
        ss << buff;
        readPos += n;
    }
    return n;
}

ssize_t TCPContext::Write(string msg) {
    ssize_t n = write(fd, msg.c_str(), msg.size());
    if (n < 0) throw TCPError(fd);
    return n;
}

void TCPContext::Close() {
    int result = close(fd);
    if (result < 0) throw TCPError(fd);
}

string TCPContext::GetCommand() {
    string cmd = ss.str();
    ss.str("");
    ss.clear();
    return cmd;
}

int TCPContext::GetReadPos() {
    return readPos;
}