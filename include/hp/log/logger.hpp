#pragma once
#include "hp/log/level.hpp"
#include "hp/log/sink.hpp"

#include <mutex>
#include <string>
#include <string_view>
#include <atomic>

namespace hp::log {

class Logger {
public:
    explicit Logger(ILogSink& sink);

    void set_level(Level lv);

    void log(Level lv, std::string_view msg);

    // 便捷方法
    void info(std::string_view msg)  { log(Level::Info, msg); }
    void warn(std::string_view msg)  { log(Level::Warn, msg); }
    void error(std::string_view msg) { log(Level::Error, msg); }

private:
    std::string format(Level lv, std::string_view msg);

private:
    ILogSink& sink_;
    std::atomic<Level> level_{Level::Info};
    std::mutex mu_;
};

} // namespace hp::log