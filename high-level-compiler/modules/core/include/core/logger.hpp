#ifndef CL_CORE_LOGGER_HPP
#define CL_CORE_LOGGER_HPP
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <string>
#include <thread>
#include <vector>

namespace core::logger {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
};

struct LogConfig {
    LogLevel min_level;
    std::size_t queue_size;
    bool colored_output;
};

class Logger {
public:
    static Logger& get_instance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    void log(LogLevel level, const std::string& msg);
    void init(const LogConfig& cfg);
    void shutdown();

    LogLevel get_min_lvl() const {return m_config.min_level;}

private:
    struct LogDecorators {
        std::string color;
        std::string tag;
    };
    struct LogEntry {
        std::string msg;
        LogLevel level;
        std::chrono::system_clock::time_point timestamp;
    };

    Logger() = default;
    ~Logger() = default;

    std::vector<LogEntry> m_queue;
    std::condition_variable m_cond;
    std::atomic<bool> m_shutdown;
    std::atomic<bool> m_flushed;
    std::thread m_worker;
    LogConfig m_config;
    std::mutex m_mutex;

    void run();
    void write(const LogEntry& entry);
    LogDecorators get_decorator(LogLevel lvl);
};

};  // namespace core::logger

#define _CL_LOG_LEVEL_TEST(lvl, ...) \
    do { \
        if (lvl >= cl::core::logger::Logger::get_instance().get_min_lvl()) { \
            cl::core::logger::Logger::get_instance().log(lvl,std::format(__VA_ARGS__));\
        }\
    } while(false)

#define LOG_TRACE(...) _CL_LOG_LEVEL_TEST(core::logger::LogLevel::TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) _CL_LOG_LEVEL_TEST(core::logger::LogLevel::DEBUG, __VA_ARGS__)
#define LOG_INFO(...) _CL_LOG_LEVEL_TEST(core::logger::LogLevel::INFO, __VA_ARGS__)
#define LOG_WARNING(...) _CL_LOG_LEVEL_TEST(core::logger::LogLevel::WARNING, __VA_ARGS__)
#define LOG_ERROR(...) _CL_LOG_LEVEL_TEST(core::logger::LogLevel::ERROR, __VA_ARGS__)
#define LOG_CRITICAL(...) _CL_LOG_LEVEL_TEST(core::logger::LogLevel::CRITICAL, __VA_ARGS__)

#endif
