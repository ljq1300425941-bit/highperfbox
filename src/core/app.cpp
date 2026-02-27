#include "hp/core/app.hpp"
#include "hp/log/logger.hpp"
#include "hp/log/console_sink.hpp"
#include <iostream>

namespace hp::core{

void App::run()
{
    hp::log::ConsoleSink sink;
    hp::log::Logger logger(sink);

    logger.info("HighPerfBox core running.");
    logger.warn("This is a warning example.");
    logger.error("This is an error example.");
}    

}