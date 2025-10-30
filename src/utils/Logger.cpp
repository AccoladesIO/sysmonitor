#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

Logger::Logger(const std::string& filename) : enabled(!filename.empty()) {
    if (enabled) {
        log_file.open(filename, std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "Failed to open log file: " << filename << "\n";
            enabled = false;
        }
    }
}

Logger::~Logger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

void Logger::log(const std::string& message, const std::string& level) {
    if (!enabled) return;
    
    std::lock_guard<std::mutex> lock(log_mutex);
    
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now_c));
    
    log_file << "[" << time_str << "] [" << level << "] " << message << "\n";
    log_file.flush();
}

void Logger::error(const std::string& message) { log(message, "ERROR"); }
void Logger::warn(const std::string& message) { log(message, "WARN"); }
void Logger::info(const std::string& message) { log(message, "INFO"); }
void Logger::debug(const std::string& message) { log(message, "DEBUG"); }