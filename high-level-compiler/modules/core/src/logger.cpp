#include "core/logger.hpp"

#include <chrono>
#include <mutex>
#include <print>

namespace core::logger {

void Logger::init(const LogConfig& cfg) {
    m_config = cfg;
    m_queue.reserve(cfg.queue_size);
    m_worker = std::thread(&Logger::run, this);
    auto decorator = get_decorator(cfg.min_level);
    log(LogLevel::INFO, std::format("Logger initialized with MIN_LOG_LEVEL={}", decorator.tag));
}

void Logger::shutdown() {
    m_shutdown = true;
    m_cond.notify_one();
    std::unique_lock lock(m_mutex);
    m_cond.wait(lock, [this]() { return m_flushed.load(); });

    if (m_worker.joinable()) m_worker.join();
}

void Logger::run() {
    while (true) {
        std::vector<LogEntry> buffer = {};
        {
            std::unique_lock lock(m_mutex);
            m_cond.wait(lock,
                        [this] { return !m_queue.empty() || m_shutdown; });
            std::swap(buffer, m_queue);
        }
        for (auto& el : buffer) write(el);
        {
            std::unique_lock lock(m_mutex);
            if (m_shutdown && m_queue.empty()) {
                m_flushed = true;
                m_cond.notify_one();
                return;
            }
        }
    }
}

void Logger::write(const LogEntry& entry) {
    std::string color = get_decorator(entry.level).color;
    std::string tag = get_decorator(entry.level).tag;

    auto local_time =
        std::chrono::zoned_time{std::chrono::current_zone(), entry.timestamp};
    auto time_str = std::format("{:%Y-%m-%d %H:%M:%S}", local_time);

    std::println("{}[{}][{}] {}\033[0m", color, tag, time_str, entry.msg);
}

void Logger::log(LogLevel level, const std::string& msg) {
    if (m_shutdown) return;
    if (m_config.min_level > level) return;
    {
        std::unique_lock lock(m_mutex);
        LogEntry entry = {
            .msg = msg,
            .level = level,
            .timestamp = std::chrono::system_clock::now(),
        };
        m_queue.push_back(entry);
    }
    m_cond.notify_one();
}

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

Logger::LogDecorators Logger::get_decorator(LogLevel lvl) {
    switch (lvl) {
        case (LogLevel::TRACE):
            return {.color = "\033[90m", .tag = "TRACE"};
        case (LogLevel::DEBUG):
            return {.color = "\033[0;36m", .tag = "DEBUG"};
        case (LogLevel::INFO):
            return {.color = "", .tag = "INFO"};
        case (LogLevel::WARNING):
            return {.color = "\033[1;33m", .tag = "WARNING"};
        case (LogLevel::ERROR):
            return {.color = "\033[1;31m", .tag = "ERROR"};
        case (LogLevel::CRITICAL):
            return {.color = "\033[1;31m", .tag = "CRITICAL"};
    }
    return {.color = "", .tag = "UNDEFINED"};
}

}  // namespace core::logger
