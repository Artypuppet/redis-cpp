#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>

#include "server/TCPServer.h"
#include "common/Exceptions.h"

int main(int argc, char **argv)
{
    CLI::App app{"redis-cpp"};

    std::string logLevel{};
    auto logOption = app.add_option("-L,--log-level", logLevel, "Log level to use");

    CLI11_PARSE(app, argc, argv);

    if(logLevel == "" || logLevel == "d") {
        spdlog::set_level(spdlog::level::debug);
    } else if (logLevel == "e") {
        spdlog::set_level(spdlog::level::err);
    } else {
        spdlog::set_level(spdlog::level::info);
    } 

    TCPServer server;
    try {
        server.SetupServer();
    } catch(TCPError& e) {
        spdlog::error("Error occurred while setuping up the server: {}", e.what());
    }
    server.EventLoop();
    return 0;
}
