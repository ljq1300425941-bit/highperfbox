#pragma once
#include <string_view>

namespace hp::log {

class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void write(std::string_view msg) = 0;
};

} // namespace hp::log