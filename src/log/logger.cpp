#include "hp/log/logger.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

namespace hp::log {

const char* to_string(Level lv) {
    switch (lv) {
        case Level::Trace: return "TRACE";
        case Level::Debug: return "DEBUG";
        case Level::Info:  return "INFO";
        case Level::Warn:  return "WARN";
        case Level::Error: return "ERROR";
        case Level::Fatal: return "FATAL";
        default:           return "UNKNOWN";
    }
}

Logger::Logger(ILogSink& sink) : sink_(sink) {}

void Logger::set_level(Level lv) {
    std::lock_guard<std::mutex> lk(mu_);
    level_ = lv;
}

void Logger::log(Level lv, std::string_view msg) {
    std::lock_guard<std::mutex> lk(mu_);
    if (static_cast<int>(lv) < static_cast<int>(level_)) return;
    sink_.write(format(lv, msg));
}

std::string Logger::format(Level lv, std::string_view msg) {
    using clock = std::chrono::system_clock;
    const auto now = clock::now();
    const auto t = clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch()) % 1000;

    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%F %T")
        << '.' << std::setw(3) << std::setfill('0') << ms.count()
        << " [" << to_string(lv) << "]"
        << " [tid:" << std::this_thread::get_id() << "] "
        << msg;
    return oss.str();
}

} // namespace hp::log