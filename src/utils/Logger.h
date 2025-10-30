#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger {
private:
    std::ofstream log_file;
    std::mutex log_mutex;
    bool enabled;
    
public:
    Logger(const std::string& filename = "");
    ~Logger();
    void log(const std::string& message, const std::string& level = "INFO");
    void error(const std::string& message);
    void warn(const std::string& message);
    void info(const std::string& message);
    void debug(const std::string& message);
};

#endif // LOGGER_H