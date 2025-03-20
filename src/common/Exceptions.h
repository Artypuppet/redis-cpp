#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>

class TCPError: public std::runtime_error {
public:
    int errCode;
    TCPError(int fd) : std::runtime_error("TCP file descriptor " + std::to_string(fd) + ": " + strerror(errno)), errCode(errno) {}
};