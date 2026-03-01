#include "hp/config/config.hpp"
#include <fstream>
#include <sstream>

namespace hp::config {

bool Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        data_[key] = value;
    }

    return true;
}

std::string Config::get_string(const std::string& key) const {
    return data_.at(key);
}

int Config::get_int(const std::string& key) const {
    return std::stoi(data_.at(key));
}

}