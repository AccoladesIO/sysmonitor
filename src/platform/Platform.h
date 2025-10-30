#ifndef PLATFORM_H
#define PLATFORM_H

#include <vector>
#include <string>

namespace Platform {
    // CPU functions
    void getCPUStats(long& total, long& idle);
    double calculateCPUUsage(long prev_total, long prev_idle, long& new_total, long& new_idle);
    
    // Memory functions
    void getMemoryInfo(long& total_kb, long& available_kb, long& used_kb);
    
    // Process functions
    struct ProcessData {
        int pid;
        std::string name;
        double cpu_usage;
        long memory_kb;
        int priority;
    };
    
    std::vector<ProcessData> getProcessList();
    bool setProcessPriority(int pid, int nice_value);
    
    // System functions
    bool isElevated();
    void sleep(int milliseconds);
    std::string getConfigDirectory();
}

#endif // PLATFORM_H