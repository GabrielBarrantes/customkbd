#include "Logger.hpp"

namespace ckbd
{

    Logger &Logger::instance()
    {
        static Logger inst;
        return inst;
    }

    void Logger::set_level(LogLevel lvl) { level_ = lvl; }

    static const char *level_to_str(LogLevel lvl)
    {
        switch (lvl)
        {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        }
        return "?";
    }

    void Logger::log(LogLevel lvl, const std::string &msg)
    {
        if (lvl < level_)
            return;
        std::lock_guard<std::mutex> lock(mtx_);
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_r(&t, &tm);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%F %T");
        std::cerr << oss.str() << " [" << level_to_str(lvl) << "] " << msg << '\n';
    }

    void Logger::debug(const std::string &msg) { log(LogLevel::Debug, msg); }
    void Logger::info(const std::string &msg) { log(LogLevel::Info, msg); }
    void Logger::warn(const std::string &msg) { log(LogLevel::Warn, msg); }
    void Logger::error(const std::string &msg) { log(LogLevel::Error, msg); }

} // namespace ckbd