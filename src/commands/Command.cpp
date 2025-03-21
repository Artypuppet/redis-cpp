#include "Command.h"

using std::string;

Command::Command(): connfd(-1) {}

Command::Command(int _fd, string _cmd): connfd(_fd), cmd(_cmd) {}

// Move constructor for efficient access from the event queue.
Command::Command(Command&& other): connfd(other.connfd), cmd(std::move(other.cmd)) {}

// Run runs the command. Does nothing for now.
void Command::Run() { return; }