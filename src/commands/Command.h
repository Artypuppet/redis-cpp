#ifndef COMMAND_H
#define COMMAND_H

#include <string>

class Command {
private:
    std::string cmd;

public:
    int connfd;
    Command();
    Command(int _fd, std::string _cmd);
    Command(Command&& other);
    std::string Run();
};

#endif