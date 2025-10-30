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
    
    ProcessInfo() : pid(0), cpu_usage(0.0), memory_kb(0), priority(0), nice_value(0) {}
};

struct SystemMetrics {
    double cpu_usage;
    long total_mem_kb;
    long used_mem_kb;
    long available_mem_kb;
    double mem_usage_percent;
    std::vector<ProcessInfo> top_processes;
    
    SystemMetrics() : cpu_usage(0.0), total_mem_kb(0), used_mem_kb(0), 
                     available_mem_kb(0), mem_usage_percent(0.0) {}
};

#endif // PROCESSINFO_H
