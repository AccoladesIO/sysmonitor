#ifdef _WIN32

#include "Platform.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <vector>
#include <string>
#include <thread>
#include <shlobj.h>
#include <algorithm>


namespace Platform {

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;
static bool pdhInitialized = false;

void initPDH() {
    if (!pdhInitialized) {
        PdhOpenQuery(NULL, NULL, &cpuQuery);
        PdhAddEnglishCounterA(cpuQuery, "\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
        PdhCollectQueryData(cpuQuery);
        pdhInitialized = true;
    }
}

void getCPUStats(long& total, long& idle) {
    initPDH();
    
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        auto fileTimeToLong = [](const FILETIME& ft) -> long {
            ULARGE_INTEGER uli;
            uli.LowPart = ft.dwLowDateTime;
            uli.HighPart = ft.dwHighDateTime;
            return static_cast<long>(uli.QuadPart / 10000);
        };
        
        idle = fileTimeToLong(idleTime);
        total = fileTimeToLong(kernelTime) + fileTimeToLong(userTime);
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
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    
    if (GlobalMemoryStatusEx(&memInfo)) {
        total_kb = static_cast<long>(memInfo.ullTotalPhys / 1024);
        available_kb = static_cast<long>(memInfo.ullAvailPhys / 1024);
        used_kb = total_kb - available_kb;
    }
}

std::vector<ProcessData> getProcessList() {
    std::vector<ProcessData> processes;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processes;
    }
    
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            ProcessData proc;
            proc.pid = pe32.th32ProcessID;
            
            // Convert wide string to narrow string
            char name[MAX_PATH];
            WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, name, MAX_PATH, NULL, NULL);
            proc.name = name;
            
            // Get process handle
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
            if (hProcess) {
                // Get memory info
                PROCESS_MEMORY_COUNTERS_EX pmc;
                if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                    proc.memory_kb = static_cast<long>(pmc.WorkingSetSize / 1024);
                }
                
                // Get CPU usage (simplified - real implementation needs tracking)
                FILETIME createTime, exitTime, kernelTime, userTime;
                if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
                    ULARGE_INTEGER kt, ut;
                    kt.LowPart = kernelTime.dwLowDateTime;
                    kt.HighPart = kernelTime.dwHighDateTime;
                    ut.LowPart = userTime.dwLowDateTime;
                    ut.HighPart = userTime.dwHighDateTime;
                    
                    // This is a simplified calculation
                    proc.cpu_usage = 0.0; // Would need previous values for accurate calculation
                }
                
                // Get priority
                proc.priority = GetPriorityClass(hProcess);
                
                CloseHandle(hProcess);
            }
            
            if (proc.memory_kb > 0) {
                processes.push_back(proc);
            }
            
        } while (Process32NextW(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    
    // Sort by memory (since CPU calculation is simplified)
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
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
    if (!hProcess) {
        return false;
    }
    
    DWORD priorityClass;
    if (nice_value >= 15) {
        priorityClass = IDLE_PRIORITY_CLASS;
    } else if (nice_value >= 10) {
        priorityClass = BELOW_NORMAL_PRIORITY_CLASS;
    } else if (nice_value >= 0) {
        priorityClass = NORMAL_PRIORITY_CLASS;
    } else if (nice_value >= -10) {
        priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
    } else {
        priorityClass = HIGH_PRIORITY_CLASS;
    }
    
    bool result = SetPriorityClass(hProcess, priorityClass);
    CloseHandle(hProcess);
    
    return result;
}

bool isElevated() {
    BOOL isAdmin = FALSE;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    
    if (AllocateAndInitializeSid(&NtAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup)) {
        
        CheckTokenMembership(NULL, AdministratorsGroup, &isAdmin);
        FreeSid(AdministratorsGroup);
    }
    
    return isAdmin == TRUE;
}

void sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

std::string getConfigDirectory() {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\SysMonitor";
    }
    return ".";
}

} // namespace Platform

#endif // _WIN32