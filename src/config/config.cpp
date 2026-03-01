#include "hp/config/config.hpp"
#include <fstream>
#include <sstream>

namespace hp::config {


static std::string trim(std::string s) {
    auto is_space = [](unsigned char c){ return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    while (!s.empty() && is_space(s.front())) s.erase(s.begin());
    while (!s.empty() && is_space(s.back()))  s.pop_back();
    return s;
}

bool Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty())
            continue;
        if (line[0] == '#')
            continue;

        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        data_[key] = value;
    }

    return true;
}

bool Config::has(const std::string&key)const{
    return data_.find(key) != data_.end();
}

std::string Config::get_string(const std::string& key) const {
    auto it = data_.find(key);
    if (it == data_.end()) throw std::runtime_error("missing config key: " + key);
    return it->second;
}

int Config::get_int(const std::string& key) const {
    return std::stoi(get_string(key));
}

}