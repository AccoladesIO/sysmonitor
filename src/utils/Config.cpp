#include "Config.h"
#include "../platform/Platform.h"
#include <iostream>
#include <fstream>
#include <limits>

Config::Config() 
    : interval(2), optimize(false), threshold(80), history_length(120),
      color_scheme("default"), graph_type("sparkline"), 
      auto_save(true), log_level("info") {}

std::string Config::getConfigPath() {
    return Platform::getConfigDirectory() + "/config.json";
}

bool Config::load(const std::string& filename) {
    std::string path = filename.empty() ? getConfigPath() : filename;
    std::ifstream in(path);
    if (!in.is_open()) return false;

    std::string key;
    while (in >> key) {
        if (key == "interval") in >> interval;
        else if (key == "optimize") in >> optimize;
        else if (key == "threshold") in >> threshold;
        else if (key == "history_length") in >> history_length;
        else if (key == "color_scheme") in >> color_scheme;
        else if (key == "graph_type") in >> graph_type;
        else if (key == "auto_save") in >> auto_save;
        else if (key == "log_level") in >> log_level;
        else in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return true;
}

bool Config::save(const std::string& filename) {
    std::string path = filename.empty() ? getConfigPath() : filename;
    std::ofstream out(path);
    if (!out.is_open()) return false;

    out << "interval " << interval << "\n"
        << "optimize " << optimize << "\n"
        << "threshold " << threshold << "\n"
        << "history_length " << history_length << "\n"
        << "color_scheme " << color_scheme << "\n"
        << "graph_type " << graph_type << "\n"
        << "auto_save " << auto_save << "\n"
        << "log_level " << log_level << "\n";
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