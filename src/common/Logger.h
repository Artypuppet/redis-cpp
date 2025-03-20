#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

void SetupAsyncLogger() {
    spdlog::init_thread_pool(8192, 1); // Queue size, worker thread count
    auto async_logger = spdlog::basic_logger_mt<spdlog::async_factory>("async_logger", "logs/async.log");

    spdlog::set_default_logger(async_logger);
    spdlog::info("Async logger setup");
}