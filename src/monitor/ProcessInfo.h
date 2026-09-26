#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <string>
#include <vector>

struct ProcessInfo {
    int pid;
    std::string name;
    double cpu_usage;
    long memory_kb;
    int priority;
    int nice_value;

    ProcessInfo();
};

struct SystemMetrics {
    double cpu_usage;
    long total_mem_kb;
    long used_mem_kb;
    long available_mem_kb;
    double mem_usage_percent;
    std::vector<ProcessInfo> top_processes;

    SystemMetrics();
};

#endif // PROCESSINFO_H