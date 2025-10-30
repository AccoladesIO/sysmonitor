#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
public:
    int interval;
    bool optimize;
    int threshold;
    int history_length;
    std::string color_scheme;
    std::string graph_type;
    bool auto_save;
    std::string log_level;
    
    Config();
    bool load(const std::string& filename = "");
    bool save(const std::string& filename = "");
    void display() const;
    void reset();
    
private:
    std::string getConfigPath();
};

#endif // CONFIG_H