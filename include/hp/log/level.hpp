#pragma once

namespace hp::log {

enum class Level {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

const char* to_string(Level lv);

} // namespace hp::log