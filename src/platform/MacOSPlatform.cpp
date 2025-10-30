#if defined(__APPLE__)

#include "Platform.h"
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/processor_info.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <libproc.h>
#include <unistd.h>
#include <pwd.h>
#include <thread>
#include <chrono>

namespace Platform {

void getCPUStats(long& total, long& idle) {
    host_cpu_load_info_data_t cpuinfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, 
                       (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
        total = cpuinfo.cpu_ticks[CPU_STATE_USER] + 
                cpuinfo.cpu_ticks[CPU_STATE_SYSTEM] +
                cpuinfo.cpu_ticks[CPU_STATE_IDLE] +
                cpuinfo.cpu_ticks[CPU_STATE_NICE];
        idle = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
    }
}

double calculateCPUUsage(long prev_total, long prev_idle, long& new_total, long& new_idle) {
    if (prev_total == 0) return 0.0;
    
    long total_diff = new_total - prev_total;
    long idle_diff = new_idle - prev_idle;
    
    if (total_diff == 0) return 0.0;
    
    return 100.0 * (1.0 - static_cast<double>(idle_diff) / total_diff);
}

void getMemoryInfo(long& total_kb, long& available_kb, long& used_kb) {
    int mib[2];
    size_t length;
    
    // Get total memory
    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    uint64_t total_bytes;
    length = sizeof(total_bytes);
    sysctl(mib, 2, &total_bytes, &length, NULL, 0);
    total_kb = static_cast<long>(total_bytes / 1024);
    
    // Get memory statistics
    vm_statistics64_data_t vm_stats;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64,
                         (host_info64_t)&vm_stats, &count) == KERN_SUCCESS) {
        
        // Get page size
        vm_size_t page_size;
        host_page_size(mach_host_self(), &page_size);
        
        // Calculate available and used memory
        uint64_t free_mem = vm_stats.free_count * page_size;
        uint64_t inactive_mem = vm_stats.inactive_count * page_size;
        
        available_kb = static_cast<long>((free_mem + inactive_mem) / 1024);
        used_kb = total_kb - available_kb;
    }
}

std::vector<ProcessData> getProcessList() {
    std::vector<ProcessData> processes;
    
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
    size_t length;
    
    if (sysctl(mib, 4, NULL, &length, NULL, 0) < 0) {
        return processes;
    }
    
    struct kinfo_proc* proc_list = (struct kinfo_proc*)malloc(length);
    if (!proc_list) return processes;
    
    if (sysctl(mib, 4, proc_list, &length, NULL, 0) < 0) {
        free(proc_list);
        return processes;
    }
    
    int proc_count = length / sizeof(struct kinfo_proc);
    
    for (int i = 0; i < proc_count; i++) {
        ProcessData proc;
        proc.pid = proc_list[i].kp_proc.p_pid;
        proc.name = proc_list[i].kp_proc.p_comm;
        
        // Get memory info
        struct proc_taskinfo ti;
        if (proc_pidinfo(proc.pid, PROC_PIDTASKINFO, 0, &ti, sizeof(ti)) > 0) {
            proc.memory_kb = static_cast<long>(ti.pti_resident_size / 1024);
        }
        
        // Simplified CPU usage
        proc.cpu_usage = 0.0;
        
        // Get priority
        proc.priority = proc_list[i].kp_proc.p_priority;
        
        if (proc.memory_kb > 0) {
            processes.push_back(proc);
        }
    }
    
    free(proc_list);
    
    // Sort by memory
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
        return false;
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

#endif // __APPLE__