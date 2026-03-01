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

    std::cout << "cache_size = " << cfg.get_int("cache_size") << "\n";
    std::cout << "threads = " << cfg.get_int("threads") << "\n";
}    

}