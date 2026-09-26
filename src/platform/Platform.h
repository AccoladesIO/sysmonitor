#ifndef PLATFORM_H
#define PLATFORM_H

#include <vector>
#include <string>

namespace Platform {
    void getCPUStats(long& total, long& idle);
    double calculateCPUUsage(long prev_total, long prev_idle, long new_total, long new_idle);

    void getMemoryInfo(long& total_kb, long& available_kb, long& used_kb);

    struct ProcessData {
        int pid = 0;
        std::string name;
        double cpu_usage = 0.0;
        long memory_kb = 0;
        int priority = 0;
    };

    std::vector<ProcessData> getProcessList();
    bool setProcessPriority(int pid, int nice_value);

    bool isElevated();
    void sleep(int milliseconds);
    std::string getConfigDirectory();
    bool ensureDirectoryExists(const std::string& path);
}

#endif // PLATFORM_H