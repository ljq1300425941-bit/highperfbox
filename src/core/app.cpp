#include "hp/core/app.hpp"
#include "hp/log/logger.hpp"
#include "hp/log/console_sink.hpp"
#include "hp/config/config.hpp"
#include <iostream>

namespace hp::core{

void App::run()
{
    hp::config::Config cfg;
    if (!cfg.load("config.ini")) {
        std::cout << "Failed to load config\n";
        return;
    }

    hp::log::ConsoleSink sink;
    hp::log::Logger logger(sink);

    logger.info("Config loaded.");

    int threads = cfg.get_int("threads");
    int cache_size = cfg.get_int("cache_size");

    logger.info(("threads=" + std::to_string(threads)).c_str());
    logger.info(("cache_size=" + std::to_string(cache_size)).c_str());
}    

}