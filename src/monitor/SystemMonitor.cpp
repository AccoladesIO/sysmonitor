#include "SystemMonitor.h"
#include "../platform/Platform.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

SystemMonitor::SystemMonitor() 
    : prev_total(0), prev_idle(0), baseline_samples(0), 
      baseline_cpu(0.0), baseline_mem(0.0) {}

SystemMetrics SystemMonitor::collectMetrics() {
    SystemMetrics metrics;
    
    long total, idle;
    Platform::getCPUStats(total, idle);
    metrics.cpu_usage = Platform::calculateCPUUsage(prev_total, prev_idle, total, idle);
    prev_total = total;
    prev_idle = idle;
    
    Platform::getMemoryInfo(metrics.total_mem_kb, metrics.available_mem_kb, metrics.used_mem_kb);
    metrics.mem_usage_percent = 100.0 * metrics.used_mem_kb / metrics.total_mem_kb;
    
    auto proc_list = Platform::getProcessList();
    for (const auto& p : proc_list) {
        ProcessInfo proc;
        proc.pid = p.pid;
        proc.name = p.name;
        proc.cpu_usage = p.cpu_usage;
        proc.memory_kb = p.memory_kb;
        proc.priority = p.priority;
        metrics.top_processes.push_back(proc);
    }
    
    std::sort(metrics.top_processes.begin(), metrics.top_processes.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return a.cpu_usage > b.cpu_usage;
              });
    
    if (metrics.top_processes.size() > 10) {
        metrics.top_processes.resize(10);
    }
    
    cpu_history.push_back(metrics.cpu_usage);
    mem_history.push_back(metrics.mem_usage_percent);
    
    if (cpu_history.size() > MAX_HISTORY) cpu_history.pop_front();
    if (mem_history.size() > MAX_HISTORY) mem_history.pop_front();
    
    return metrics;
}

void SystemMonitor::establishBaseline(int samples) {
    std::cout << "Establishing baseline metrics..." << std::flush;
    
    double total_cpu = 0.0;
    double total_mem = 0.0;
    
    for (int i = 0; i < samples; i++) {
        auto metrics = collectMetrics();
        total_cpu += metrics.cpu_usage;
        total_mem += metrics.mem_usage_percent;
        std::cout << "." << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    baseline_cpu = total_cpu / samples;
    baseline_mem = total_mem / samples;
    baseline_samples = samples;
    
    std::cout << " Done!\n";
    std::cout << "Baseline CPU: " << baseline_cpu << "%\n";
    std::cout << "Baseline Memory: " << baseline_mem << "%\n\n";
}