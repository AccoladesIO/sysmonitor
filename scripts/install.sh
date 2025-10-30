echo "╔════════════════════════════════════════════════════════╗"
echo "║  SysMonitor Installation Script                        ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found. Please install CMake first."
    echo "   Ubuntu/Debian: sudo apt install cmake"
    echo "   macOS: brew install cmake"
    exit 1
fi

# Check for compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "❌ C++ compiler not found. Please install build tools."
    echo "   Ubuntu/Debian: sudo apt install build-essential"
    echo "   macOS: xcode-select --install"
    exit 1
fi

echo "✓ Prerequisites check passed"
echo ""

# Build
echo "Building SysMonitor..."
mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed"
    exit 1
fi

make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
if [ $? -ne 0 ]; then
    echo "❌ Build failed"
    exit 1
fi

echo "✓ Build successful"
echo ""

# Install
echo "Installing SysMonitor..."
if [ "$EUID" -ne 0 ]; then
    echo "⚠  Installation requires sudo privileges"
    sudo make install
else
    make install
fi

if [ $? -eq 0 ]; then
    echo "✓ Installation successful"
    echo ""
    echo "You can now run: sysmonitor start"
    echo "For help: sysmonitor --help"
else
    echo "❌ Installation failed"
    exit 1
fi