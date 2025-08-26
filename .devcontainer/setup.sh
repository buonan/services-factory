#!/bin/bash

# Devcontainer setup script for C++ Services Framework
# This script runs after the container is created to set up the development environment

set -e

echo "🚀 Setting up C++ Services Framework development environment..."

# Verify Bazel installation
echo "📦 Verifying Bazel installation..."
bazel version

# Set up Bazel completion (if available)
if command -v bazel &> /dev/null; then
    echo "🔧 Setting up Bazel completion..."
    # Add Bazel completion to bashrc if not already present
    if ! grep -q "bazel completion" ~/.bashrc; then
        echo 'source <(bazel completion bash)' >> ~/.bashrc
    fi
    
    # Add Bazel completion to zshrc if zsh is available
    if command -v zsh &> /dev/null && [ -f ~/.zshrc ]; then
        if ! grep -q "bazel completion" ~/.zshrc; then
            echo 'source <(bazel completion zsh)' >> ~/.zshrc
        fi
    fi
fi

# Create useful aliases
echo "🔗 Setting up build aliases..."
cat >> ~/.bashrc << 'EOF'

# C++ Services Framework Build Aliases
alias build-all='bazel build //...'
alias build-framework='bazel build //framework:framework_core'
alias build-services='bazel build //services:all_services'
alias run-demo='bazel run //:service_framework_demo'
alias run-rest-api='bazel run //services/rest_api:rest_api_demo'
alias run-weather='bazel run //services/weather:weather_main'
alias run-tests='bazel test //framework:framework_test'
alias bazel-clean='bazel clean'
alias bazel-info='bazel info'

# Make and CMake aliases for comparison
alias make-all='make all'
alias make-demo='make run-demo'
alias cmake-build='mkdir -p build && cd build && cmake .. && make'

EOF

# Set up git configuration if not already configured
if [ ! -f ~/.gitconfig ]; then
    echo "📝 Setting up basic git configuration..."
    git config --global init.defaultBranch main
    git config --global core.editor vim
    git config --global pull.rebase false
fi

# Create workspace directories
echo "📁 Setting up workspace directories..."
mkdir -p ~/workspace/build-cache
mkdir -p ~/workspace/logs

# Set up Bazel user configuration
echo "⚙️  Setting up Bazel user configuration..."
mkdir -p ~/.bazelrc.d
cat > ~/.bazelrc << 'EOF'
# User-specific Bazel configuration
build --disk_cache=~/workspace/build-cache
build --repository_cache=~/workspace/build-cache/repos

# Enable build event service for better debugging
build --build_event_text_file=~/workspace/logs/build_events.txt

# Improve build performance
build --local_ram_resources=HOST_RAM*0.75
build --local_cpu_resources=HOST_CPUS*0.75

# Better error reporting
build --verbose_failures
build --show_timestamps

# Color output
build --color=yes

EOF

# Test the build system
echo "🧪 Testing build systems..."

echo "  Testing Bazel build..."
if bazel build //framework:service_interface; then
    echo "  ✅ Bazel build successful"
else
    echo "  ❌ Bazel build failed"
fi

echo "  Testing Make build..."
if make --version > /dev/null 2>&1; then
    echo "  ✅ Make available"
else
    echo "  ❌ Make not available"
fi

echo "  Testing CMake build..."
if cmake --version > /dev/null 2>&1; then
    echo "  ✅ CMake available"
else
    echo "  ❌ CMake not available"
fi

# Display helpful information
echo ""
echo "🎉 Development environment setup complete!"
echo ""
echo "📋 Available commands:"
echo "  build-all      - Build all targets with Bazel"
echo "  run-demo       - Run the main framework demo"
echo "  run-rest-api   - Run the REST API demo"
echo "  run-weather    - Run the weather service demo"
echo "  run-tests      - Run framework tests"
echo ""
echo "📚 Build system options:"
echo "  Bazel:  ./build_bazel.sh  or  bazel build //..."
echo "  Make:   make all"
echo "  CMake:  cmake-build"
echo ""
echo "📖 Documentation:"
echo "  README.md  - General project documentation"
echo "  BAZEL.md   - Bazel-specific build guide"
echo ""
echo "🔄 Reload your shell or run 'source ~/.bashrc' to use the new aliases"
