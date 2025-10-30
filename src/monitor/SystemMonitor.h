#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include "ProcessInfo.h"
#include <map>
#include <deque>
#include <vector>

class SystemMonitor {
private:
    long prev_total;
    long prev_idle;
    std::map<int, std::pair<long, long>> prev_proc_stats;
    int baseline_samples;
    double baseline_cpu;
    double baseline_mem;
    std::deque<double> cpu_history;
    std::deque<double> mem_history;
    static const int MAX_HISTORY = 120;
    
    void getCPUStats(long& total, long& idle);
    double calculateCPUUsage();
    void getMemoryInfo(long& total, long& available, long& used);
    std::vector<ProcessInfo> getTopProcesses(int count = 10);
    
public:
    SystemMonitor();
    SystemMetrics collectMetrics();
    void establishBaseline(int samples = 5);
    double getBaselineCPU() const { return baseline_cpu; }
    double getBaselineMem() const { return baseline_mem; }
    const std::deque<double>& getCPUHistory() const { return cpu_history; }
    const std::deque<double>& getMemHistory() const { return mem_history; }
};

#endif // SYSTEMMONITOR_H