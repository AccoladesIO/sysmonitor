#include "./Optimizer.h"
#include "../platform/Platform.h"

Optimizer::Optimizer(int threshold) : cpu_threshold(threshold) {}

std::vector<ProcessInfo> Optimizer::optimizeProcesses(const std::vector<ProcessInfo>& processes) {
    std::vector<ProcessInfo> optimized;
    
    for (const auto& proc : processes) {
        if (proc.cpu_usage > cpu_threshold && proc.priority < 10) {
            if (optimizeProcess(proc.pid, 10)) {
                optimized.push_back(proc);
            }
        }
    }
    
    return optimized;
}

bool Optimizer::optimizeProcess(int pid, int nice_value) {
    return Platform::setProcessPriority(pid, nice_value);
}

void Optimizer::setCPUThreshold(int threshold) {
    cpu_threshold = threshold;
}

int Optimizer::getCPUThreshold() const { return cpu_threshold; }