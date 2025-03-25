#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>
#include <format>

class TCPError: public std::runtime_error {
public:
    int errCode;
    TCPError(int fd) : std::runtime_error(std::format("TCP file descriptor {}:  {}", fd, strerror(errno))), errCode(errno) {}
    TCPError(int fd, std::string msg) : std::runtime_error(std::format("TCP file descriptor {} resulted in error: {}", fd, msg)) {}
};