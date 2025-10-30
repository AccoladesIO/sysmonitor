# Simple PowerShell build script for SysMonitor
Write-Host "Building SysMonitor..." -ForegroundColor Cyan

$sources = @(
    "src/main.cpp"
    "src/monitor/SystemMonitor.cpp"
    "src/monitor/ProcessInfo.cpp"
    "src/visualizer/Visualizer.cpp"
    "src/optimizer/Optimizer.cpp"
    "src/utils/Config.cpp"
    "src/utils/Logger.cpp"
    "src/platform/WindowsPlatform.cpp"
)

# Build
& g++ -std=c++11 -O3 `
    -Iinclude -Isrc `
    $sources `
    -lpsapi -lpdh `
    -Wl,--subsystem,console `
    -o sysmonitor.exe 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n? BUILD SUCCESSFUL!" -ForegroundColor Green
    Write-Host "Run: .\sysmonitor.exe start" -ForegroundColor Yellow
} else {
    Write-Host "`n? Build failed" -ForegroundColor Red
}
