#if defined(__linux__)

#include "Platform.h"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <pwd.h>
#include <thread>
#include <chrono>
#include <algorithm>

namespace Platform {

void getCPUStats(long& total, long& idle) {
    std::ifstream stat("/proc/stat");
    std::string line;
    std::getline(stat, line);
    
    std::istringstream ss(line);
    std::string cpu;
    long user, nice, system, idle_time, iowait, irq, softirq;
    
    ss >> cpu >> user >> nice >> system >> idle_time >> iowait >> irq >> softirq;
    
    idle = idle_time + iowait;
    total = user + nice + system + idle_time + iowait + irq + softirq;
}

double calculateCPUUsage(long prev_total, long prev_idle, long& new_total, long& new_idle) {
    if (prev_total == 0) return 0.0;
    
    long total_diff = new_total - prev_total;
    long idle_diff = new_idle - prev_idle;
    
    if (total_diff == 0) return 0.0;
    
    return 100.0 * (1.0 - static_cast<double>(idle_diff) / total_diff);
}

void getMemoryInfo(long& total_kb, long& available_kb, long& used_kb) {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    
    long mem_free = 0, buffers = 0, cached = 0;
    
    while (std::getline(meminfo, line)) {
        std::istringstream ss(line);
        std::string key;
        long value;
        
        ss >> key >> value;
        
        if (key == "MemTotal:") total_kb = value;
        else if (key == "MemFree:") mem_free = value;
        else if (key == "MemAvailable:") available_kb = value;
        else if (key == "Buffers:") buffers = value;
        else if (key == "Cached:") cached = value;
    }
    
    used_kb = total_kb - mem_free - buffers - cached;
}

std::vector<ProcessData> getProcessList() {
    std::vector<ProcessData> processes;
    DIR* dir = opendir("/proc");
    
    if (!dir) return processes;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (!isdigit(entry->d_name[0])) continue;
        
        ProcessData proc;
        proc.pid = std::stoi(entry->d_name);
        
        // Get process name
        std::ifstream comm("/proc/" + std::string(entry->d_name) + "/comm");
        std::getline(comm, proc.name);
        
        // Get memory usage
        std::ifstream status("/proc/" + std::string(entry->d_name) + "/status");
        std::string line;
        while (std::getline(status, line)) {
            if (line.substr(0, 6) == "VmRSS:") {
                std::istringstream ss(line.substr(6));
                ss >> proc.memory_kb;
                break;
            }
        }
        
        // Get CPU usage (simplified)
        std::ifstream stat("/proc/" + std::string(entry->d_name) + "/stat");
        std::string stat_data;
        std::getline(stat, stat_data);
        
        std::istringstream ss(stat_data);
        std::string temp;
        long utime, stime;
        
        for (int i = 0; i < 13; i++) ss >> temp;
        ss >> utime >> stime;
        
        // Simplified CPU calculation
        proc.cpu_usage = (utime + stime) / 100.0;
        
        // Get priority
        for (int i = 0; i < 4; i++) ss >> temp;
        ss >> proc.priority;
        
        if (proc.memory_kb > 0) {
            processes.push_back(proc);
        }
    }
    
    closedir(dir);
    
    // Sort by CPU usage
    std::sort(processes.begin(), processes.end(),
              [](const ProcessData& a, const ProcessData& b) {
                  return a.memory_kb > b.memory_kb;
              });
    
    if (processes.size() > 10) {
        processes.resize(10);
    }
    
    return processes;
}

bool setProcessPriority(int pid, int nice_value) {
    // Check if we have permission
    if (getuid() != 0 && nice_value < 0) {
        return false; // Need root for negative nice values
    }
    
    return setpriority(PRIO_PROCESS, pid, nice_value) == 0;
}

bool isElevated() {
    return getuid() == 0;
}

void sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

std::string getConfigDirectory() {
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/.config/sysmonitor";
    }
    
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        return std::string(pw->pw_dir) + "/.config/sysmonitor";
    }
    
    return ".";
}

} // namespace Platform

#endif // __linux__