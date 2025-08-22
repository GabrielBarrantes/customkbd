#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace ckbd
{

    enum class LogLevel
    {
        Debug,
        Info,
        Warn,
        Error
    };

    class Logger
    {
    public:
        static Logger &instance();
        void set_level(LogLevel lvl);
        void debug(const std::string &msg);
        void info(const std::string &msg);
        void warn(const std::string &msg);
        void error(const std::string &msg);

    private:
        Logger() = default;
        std::mutex mtx_;
        LogLevel level_ = LogLevel::Info;
        void log(LogLevel lvl, const std::string &msg);
    };

} // namespace ckbd