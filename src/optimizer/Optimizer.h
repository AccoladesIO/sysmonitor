#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "../monitor/ProcessInfo.h"
#include <vector>

class Optimizer {
private:
    int cpu_threshold;

public:
    Optimizer(int threshold = 80);
    std::vector<ProcessInfo> optimizeProcesses(const std::vector<ProcessInfo>& processes);
    bool optimizeProcess(int pid, int nice_value = 10);
    void setCPUThreshold(int threshold);
    int getCPUThreshold() const;
};

#endif // OPTIMIZER_H