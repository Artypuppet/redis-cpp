#ifndef COMMAND_H
#define COMMAND_h

#include <string>

class Command {
private:
    int connfd;
    std::string cmd;

public:
    Command();
    Command(int _fd, std::string _cmd);
    Command(Command&& other);
    void Run();
};

#endif COMMAND_H