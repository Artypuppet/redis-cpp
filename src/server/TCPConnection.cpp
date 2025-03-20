#include <string>
#include <unistd.h>
#include <TCPConnection.h>
#include <common/Exceptions.h>

using std::string;

TCPConnection::TCPConnection() : fd(-1), readPos(-1) {};

TCPConnection::TCPConnection(int _fd) : fd(_fd), readPos(-1) {};

ssize_t TCPConnection::Read() {
    ssize_t n = read(fd, buff, BUFF_SIZE);
    if (n < 0) throw TCPError(fd);
    else if (n == 0) return n;
    ss << buff;
    return n;
}

int TCPConnection::Write(string& msg) {
    ssize_t n = write(fd, msg.c_str(), msg.size());
    if (n < 0) throw TCPError(fd);
    return n;
}

int TCPConnection::Close() {
    int result = close(fd);
    if (result < 0) throw TCPError(fd);
}

string TCPConnection::GetCommand() {
    return ss.str();
}