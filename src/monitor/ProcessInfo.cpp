#include "ProcessInfo.h"

ProcessInfo::ProcessInfo()
    : pid(0), cpu_usage(0.0), memory_kb(0), priority(0), nice_value(0) {}

SystemMetrics::SystemMetrics()
    : cpu_usage(0.0), total_mem_kb(0), used_mem_kb(0),
      available_mem_kb(0), mem_usage_percent(0.0) {}