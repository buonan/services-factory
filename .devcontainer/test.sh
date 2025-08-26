#!/bin/bash

# Test script to verify devcontainer setup
# This script can be run to verify that all tools are properly installed

set -e

echo "🧪 Testing C++ Services Framework Devcontainer Setup"
echo "=================================================="

# Test basic tools
echo "📋 Checking basic tools..."
echo -n "  GCC: "
gcc --version | head -n1
echo -n "  G++: "
g++ --version | head -n1
echo -n "  Make: "
make --version | head -n1
echo -n "  CMake: "
cmake --version | head -n1
echo -n "  Git: "
git --version
echo -n "  Bazel: "
bazel version | head -n1

# Test Bazel build
echo ""
echo "🏗️  Testing Bazel build..."
if bazel build //framework:service_interface; then
    echo "  ✅ Bazel build successful"
else
    echo "  ❌ Bazel build failed"
    exit 1
fi

# Test Make build
echo ""
echo "🔨 Testing Make build..."
if make --version > /dev/null 2>&1; then
    echo "  ✅ Make available"
    # Test if Makefile exists and can be parsed
    if make -n all > /dev/null 2>&1; then
        echo "  ✅ Makefile valid"
    else
        echo "  ⚠️  Makefile may have issues"
    fi
else
    echo "  ❌ Make not available"
fi

# Test CMake
echo ""
echo "🏗️  Testing CMake..."
if cmake --version > /dev/null 2>&1; then
    echo "  ✅ CMake available"
    # Test if CMakeLists.txt exists
    if [ -f "CMakeLists.txt" ]; then
        echo "  ✅ CMakeLists.txt found"
    else
        echo "  ⚠️  CMakeLists.txt not found"
    fi
else
    echo "  ❌ CMake not available"
fi

# Test Java (required for Bazel)
echo ""
echo "☕ Testing Java (required for Bazel)..."
if java -version > /dev/null 2>&1; then
    echo -n "  Java: "
    java -version 2>&1 | head -n1
    echo "  ✅ Java available"
else
    echo "  ❌ Java not available"
fi

# Test shell aliases
echo ""
echo "🔗 Testing shell aliases..."
if command -v build-all > /dev/null 2>&1; then
    echo "  ✅ Build aliases configured"
else
    echo "  ⚠️  Build aliases not configured (run 'source ~/.bashrc')"
fi

# Test VS Code integration files
echo ""
echo "🎨 Testing VS Code integration..."
if [ -f ".vscode/tasks.json" ]; then
    echo "  ✅ VS Code tasks configured"
else
    echo "  ❌ VS Code tasks not found"
fi

if [ -f ".vscode/launch.json" ]; then
    echo "  ✅ VS Code debug configurations available"
else
    echo "  ❌ VS Code debug configurations not found"
fi

# Test project structure
echo ""
echo "📁 Testing project structure..."
required_files=(
    "WORKSPACE"
    "MODULE.bazel"
    ".bazelrc"
    "BUILD"
    "framework/BUILD"
    "services/BUILD"
    "Makefile"
    "CMakeLists.txt"
)

for file in "${required_files[@]}"; do
    if [ -f "$file" ]; then
        echo "  ✅ $file"
    else
        echo "  ❌ $file missing"
    fi
done

# Test build systems
echo ""
echo "🚀 Testing quick builds..."

echo "  Testing Bazel quick build..."
if timeout 30s bazel build //framework:service_interface > /dev/null 2>&1; then
    echo "  ✅ Bazel quick build successful"
else
    echo "  ⚠️  Bazel quick build timed out or failed"
fi

echo ""
echo "🎉 Devcontainer test completed!"
echo ""
echo "📋 Next steps:"
echo "  1. Run 'source ~/.bashrc' to load aliases"
echo "  2. Try 'build-all' to build everything"
echo "  3. Try 'run-demo' to run the main demo"
echo "  4. Use VS Code tasks (Ctrl+Shift+P -> Tasks: Run Task)"
echo "  5. Use F5 to debug applications"
