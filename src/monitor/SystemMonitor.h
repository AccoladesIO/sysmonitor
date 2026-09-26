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

public:
    SystemMonitor();
    SystemMetrics collectMetrics();
    void establishBaseline(int samples = 5);
    double getBaselineCPU() const;
    double getBaselineMem() const;
    const std::deque<double>& getCPUHistory() const;
    const std::deque<double>& getMemHistory() const;
};

#endif // SYSTEMMONITOR_H