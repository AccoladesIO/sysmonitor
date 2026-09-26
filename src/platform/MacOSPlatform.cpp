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
#include <vector>
#include <cerrno>
#include <sys/stat.h>

namespace Platform {

void getCPUStats(long& total, long& idle) {
    total = 0;
    idle = 0;

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

double calculateCPUUsage(long prev_total, long prev_idle, long new_total, long new_idle) {
    if (prev_total == 0) return 0.0;

    long total_diff = new_total - prev_total;
    long idle_diff = new_idle - prev_idle;

    if (total_diff == 0) return 0.0;

    return 100.0 * (1.0 - static_cast<double>(idle_diff) / total_diff);
}

void getMemoryInfo(long& total_kb, long& available_kb, long& used_kb) {
    total_kb = 0;
    available_kb = 0;
    used_kb = 0;

    int mib[2];
    size_t length;

    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    uint64_t total_bytes = 0;
    length = sizeof(total_bytes);
    sysctl(mib, 2, &total_bytes, &length, NULL, 0);
    total_kb = static_cast<long>(total_bytes / 1024);

    vm_statistics64_data_t vm_stats;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;

    if (host_statistics64(mach_host_self(), HOST_VM_INFO64,
                         (host_info64_t)&vm_stats, &count) == KERN_SUCCESS) {

        vm_size_t page_size;
        host_page_size(mach_host_self(), &page_size);

        uint64_t free_mem = vm_stats.free_count * page_size;
        uint64_t inactive_mem = vm_stats.inactive_count * page_size;

        available_kb = static_cast<long>((free_mem + inactive_mem) / 1024);
        used_kb = total_kb - available_kb;
    }
}

std::vector<ProcessData> getProcessList() {
    std::vector<ProcessData> processes;

    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};


    std::vector<char> buffer;
    size_t length = 0;
    bool ok = false;

    for (int attempt = 0; attempt < 3; attempt++) {
        if (sysctl(mib, 4, NULL, &length, NULL, 0) < 0) {
            return processes;
        }
        length += length / 10 + sizeof(struct kinfo_proc);
        buffer.resize(length);

        if (sysctl(mib, 4, buffer.data(), &length, NULL, 0) == 0) {
            ok = true;
            break;
        }
        if (errno != ENOMEM) {
            return processes;
        }
    }

    if (!ok) return processes;

    struct kinfo_proc* proc_list = reinterpret_cast<struct kinfo_proc*>(buffer.data());
    int proc_count = static_cast<int>(length / sizeof(struct kinfo_proc));

    for (int i = 0; i < proc_count; i++) {
        ProcessData proc;
        proc.pid = proc_list[i].kp_proc.p_pid;
        proc.name = proc_list[i].kp_proc.p_comm;

        struct proc_taskinfo ti;
        if (proc_pidinfo(proc.pid, PROC_PIDTASKINFO, 0, &ti, sizeof(ti)) > 0) {
            proc.memory_kb = static_cast<long>(ti.pti_resident_size / 1024);
        }

        proc.cpu_usage = 0.0;

        proc.priority = proc_list[i].kp_proc.p_priority;

        if (proc.memory_kb > 0) {
            processes.push_back(proc);
        }
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

bool ensureDirectoryExists(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return mkdir(path.c_str(), 0755) == 0;
}

} 

#endif // __APPLE__