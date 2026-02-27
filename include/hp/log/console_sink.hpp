#pragma once
#include "hp/log/sink.hpp"

namespace hp::log {

class ConsoleSink final : public ILogSink {
public:
    void write(std::string_view msg) override;
};

} // namespace hp::log