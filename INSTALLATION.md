# Installation Guide - SysMonitor

Complete step-by-step installation instructions for Windows, Linux, and macOS.

---

## 📋 Table of Contents

1. [Windows Installation](#windows-installation)
2. [Linux Installation](#linux-installation)
3. [macOS Installation](#macos-installation)
4. [Verification](#verification)
5. [Usage Examples](#usage-examples)
6. [Troubleshooting](#troubleshooting)

---

## 🪟 Windows Installation

### Method 1: Using Visual Studio (Recommended)

#### Step 1: Install Prerequisites

**A. Install Visual Studio 2019 or later**
- Download from: https://visualstudio.microsoft.com/downloads/
- Select "Desktop development with C++" workload
- Install CMake (included with VS, or check "C++ CMake tools")

**B. Install Git**
- Download from: https://git-scm.com/download/win
- Use default options during installation

#### Step 2: Clone Repository

```cmd
# Open Command Prompt or PowerShell
cd C:\
git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor
```

#### Step 3: Build

```cmd
# Create build directory
mkdir build
cd build

# Configure with Visual Studio
cmake .. -G "Visual Studio 16 2019" -A x64

# Build
cmake --build . --config Release
```

#### Step 4: Install

```cmd
# Run as Administrator
cmake --install . --prefix "C:\Program Files\SysMonitor"
```

#### Step 5: Add to PATH

1. Press `Win + X` → Select "System"
2. Click "Advanced system settings"
3. Click "Environment Variables"
4. Under "System variables", find "Path"
5. Click "Edit" → "New"
6. Add: `C:\Program Files\SysMonitor\bin`
7. Click "OK" on all dialogs

### Method 2: Using MinGW

#### Step 1: Install MinGW-w64

```cmd
# Using Chocolatey (if installed)
choco install mingw

# Or download from: https://www.mingw-w64.org/downloads/
```

#### Step 2: Install CMake

```cmd
choco install cmake
# Or download from: https://cmake.org/download/
```

#### Step 3: Build

```cmd
cd C:\
git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor

mkdir build
cd build

cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

#### Step 4: Copy Executable

```cmd
copy bin\sysmonitor.exe C:\Windows\System32\
```

### Quick Install Script

Save this as `install.bat` and run as Administrator:

```batch
@echo off
git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
cmake --install . --prefix "C:\Program Files\SysMonitor"
echo Installation complete!
pause
```

---

## 🐧 Linux Installation

### Ubuntu/Debian

#### Step 1: Install Build Tools

```bash
sudo apt update
sudo apt install -y build-essential cmake git
```

#### Step 2: Clone and Build

```bash
cd ~
git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Step 3: Install

```bash
sudo make install
```

The binary will be installed to `/usr/local/bin/sysmonitor`

### Fedora/RHEL/CentOS

```bash
# Install dependencies
sudo dnf install -y gcc-c++ cmake git

# Build and install
git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

### Arch Linux

```bash
# Install dependencies
sudo pacman -S base-devel cmake git

# Build and install
git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

### Quick Install Script

Save as `install.sh`:

```bash
#!/bin/bash
sudo apt install -y build-essential cmake git || \
sudo dnf install -y gcc-c++ cmake git || \
sudo pacman -S base-devel cmake git

git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install

echo "✓ Installation complete!"
echo "Run: sysmonitor start"
```

Make executable and run:
```bash
chmod +x install.sh
./install.sh
```

---

## 🍎 macOS Installation

### Step 1: Install Xcode Command Line Tools

```bash
xcode-select --install
```

### Step 2: Install Homebrew (if not installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Step 3: Install CMake

```bash
brew install cmake
```

### Step 4: Clone and Build

```bash
cd ~
git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

### Step 5: Install

```bash
sudo make install
```

### Quick Install Script

```bash
#!/bin/bash
xcode-select --install
brew install cmake git

git clone https://github.com/accoladesio/sysmonitor.git
cd sysmonitor
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
sudo make install

echo "✓ Installation complete!"
echo "Run: sysmonitor start"
```

---

## ✅ Verification

After installation, verify it works:

```bash
# Check version
sysmonitor --version

# Should output: SysMonitor version 2.0.0
```

```bash
# View help
sysmonitor --help
```

```bash
# Test basic monitoring
sysmonitor start -i 5
# Press Ctrl+C to exit after a few seconds
```

---

## 🚀 Usage Examples

### Basic Monitoring

```bash
# Start with default settings (2-second updates)
sysmonitor start
```

### With Optimization

```bash
# Windows (as Administrator)
sysmonitor start --optimize

# Linux/macOS
sudo sysmonitor start --optimize
```

### Custom Settings

```bash
# Update every 5 seconds with optimization
sudo sysmonitor start -i 5 -o

# Set CPU threshold to 70%
sudo sysmonitor start -o -t 70

# Quiet mode with logging
sudo sysmonitor start -o -q -l monitor.log
```

### Shortcuts During Monitoring

- `q` or `Ctrl+C` - Exit
- `o` - Toggle optimization
- `r` - Reset baseline
- `p` - Pause/Resume
- `h` - Show help
- `+/-` - Adjust interval
- `s` - Save snapshot

---

## 🔧 Troubleshooting

### Windows Issues

**Problem: "cmake not found"**
```cmd
# Add CMake to PATH or reinstall Visual Studio with CMake tools
# Or install via Chocolatey:
choco install cmake
```

**Problem: "MSVCR120.dll missing"**
```cmd
# Install Visual C++ Redistributable
# Download from: https://support.microsoft.com/help/2977003
```

**Problem: Optimization not working**
```cmd
# Run Command Prompt as Administrator
# Right-click → "Run as administrator"
sysmonitor start -o
```

### Linux Issues

**Problem: "Permission denied" for optimization**
```bash
# Use sudo
sudo sysmonitor start -o

# Or add capability (persistent)
sudo setcap cap_sys_nice=eip /usr/local/bin/sysmonitor
sysmonitor start -o
```

**Problem: "libstdc++.so.6: version not found"**
```bash
# Update GCC
sudo apt install --reinstall libstdc++6
```

**Problem: Build fails with "C++11 required"**
```bash
# Ensure GCC 4.8+ or Clang 3.3+
gcc --version

# Update if needed
sudo apt install gcc-9 g++-9
```

### macOS Issues

**Problem: "Operation not permitted"**
```bash
# Grant Full Disk Access:
# System Preferences → Security & Privacy → Privacy → Full Disk Access
# Add Terminal/iTerm2
```

**Problem: "xcrun: error: invalid active developer path"**
```bash
# Install Xcode Command Line Tools
xcode-select --install
```

### General Issues

**Problem: High CPU usage from sysmonitor itself**
```bash
# Increase update interval
sysmonitor start -i 5

# Or use quiet mode
sysmonitor start -q
```

**Problem: Colors not displaying**
```bash
# Your terminal may not support ANSI colors
# Use: Windows Terminal, iTerm2, or GNOME Terminal

# Set terminal type
export TERM=xterm-256color
```

**Problem: Process list empty**
```bash
# Ensure /proc filesystem is available (Linux)
# Or run with appropriate permissions

# Windows: Run as Administrator
# Linux/macOS: Use sudo
```

---

## 📦 Uninstallation

### Windows

```cmd
# Remove from Program Files
rmdir /s "C:\Program Files\SysMonitor"

# Remove from PATH (follow Step 5 in reverse)
```

### Linux/macOS

```bash
sudo rm /usr/local/bin/sysmonitor
sudo rm -rf ~/.config/sysmonitor
```

---

## 🔄 Updating

```bash
cd ~/sysmonitor  # or wherever you cloned it
git pull origin main

cd build
cmake --build . --config Release
sudo make install  # or cmake --install on Windows
```

---

## 📞 Getting Help

- **Issues**: https://github.com/accoladesio/sysmonitor/issues
- **Discussions**: https://github.com/accoladesio/sysmonitor/discussions
- **Email**: your.email@example.com

---

## 🎉 Next Steps

After installation:

1. Read the [README.md](README.md) for features and usage
2. Check [docs/API.md](docs/API.md) for advanced configuration
3. Join discussions for tips and tricks
4. Star the repository if you find it useful!

**Happy Monitoring! 🚀**