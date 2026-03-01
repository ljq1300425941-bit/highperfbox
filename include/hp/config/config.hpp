#pragma once
#include <string>
#include <unordered_map>

namespace hp::config {

class Config {
public:
    bool load(const std::string& filename);

    std::string get_string(const std::string& key) const;
    int get_int(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> data_;
};

} // namespace hp::config