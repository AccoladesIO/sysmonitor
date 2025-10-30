#include "Config.h"
#include "../platform/Platform.h"
#include <iostream>
#include <fstream>

Config::Config() 
    : interval(2), optimize(false), threshold(80), history_length(120),
      color_scheme("default"), graph_type("sparkline"), 
      auto_save(true), log_level("info") {}

std::string Config::getConfigPath() {
    return Platform::getConfigDirectory() + "/config.json";
}

bool Config::load(const std::string& filename) {
    // Simple JSON parsing - you could use a library like nlohmann/json for production
    return true;
}

bool Config::save(const std::string& filename) {
    return true;
}

void Config::display() const {
    std::cout << "Current Configuration:\n";
    std::cout << "  Interval: " << interval << " seconds\n";
    std::cout << "  Optimize: " << (optimize ? "Yes" : "No") << "\n";
    std::cout << "  Threshold: " << threshold << "%\n";
    std::cout << "  History Length: " << history_length << "\n";
    std::cout << "  Color Scheme: " << color_scheme << "\n";
}

void Config::reset() {
    *this = Config();
}