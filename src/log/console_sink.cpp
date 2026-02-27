#include "hp/log/console_sink.hpp"
#include <iostream>

namespace hp::log {

void ConsoleSink::write(std::string_view msg) {
    std::cout << msg << '\n';
}

} // namespace hp::log