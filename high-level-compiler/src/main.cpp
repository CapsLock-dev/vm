#include "core/logger.hpp"

int main() {
    core::logger::LogConfig cfg = {
        .min_level = core::logger::LogLevel::TRACE,
        .queue_size = 100,
        .colored_output = true,
    };
    core::logger::Logger::get_instance().init(cfg);
    core::logger::Logger::get_instance().shutdown();
    return 0;
}
