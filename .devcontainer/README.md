# C++ Services Framework Devcontainer

This devcontainer provides a complete development environment for the C++ Services Framework with support for multiple build systems including Bazel, Make, and CMake.

## Features

### 🛠️ **Build Systems**
- **Bazel 8.x** - Modern, scalable build system with dependency management
- **Make** - Traditional build system for quick builds
- **CMake** - Cross-platform build system generator

### 🔧 **Development Tools**
- **GCC/G++** - C++17 compiler
- **Clang/Clangd** - Language server for IntelliSense
- **GDB** - Debugger
- **Valgrind** - Memory debugging
- **Git** - Version control
- **Vim** - Text editor

### 🎨 **VS Code Extensions**
- **Bazel** - Bazel build system support
- **C/C++** - IntelliSense, debugging, and code browsing
- **CMake Tools** - CMake integration
- **Makefile Tools** - Makefile support
- **Clangd** - Advanced C++ language server

### 🚀 **Shell Enhancements**
- **Zsh** with Oh My Zsh - Enhanced shell experience
- **Bazel completion** - Tab completion for Bazel commands
- **Custom aliases** - Quick access to common build commands

## Quick Start

### 1. Open in VS Code
```bash
# Open the project in VS Code
code .

# Or use the Command Palette
# Ctrl+Shift+P -> "Dev Containers: Reopen in Container"
```

### 2. Wait for Container Setup
The container will automatically:
- Install all dependencies
- Set up Bazel, Make, and CMake
- Configure VS Code extensions
- Run the setup script

### 3. Start Building
```bash
# Using Bazel (recommended)
bazel build //...
bazel run //:service_framework_demo

# Using Make
make all
make run-demo

# Using CMake
mkdir build && cd build
cmake .. && make
```

## Available Commands

### 🏗️ **Build Aliases**
```bash
build-all          # bazel build //...
build-framework    # bazel build //framework:framework_core
build-services     # bazel build //services:all_services
```

### 🏃 **Run Aliases**
```bash
run-demo          # bazel run //:service_framework_demo
run-rest-api      # bazel run //services/rest_api:rest_api_demo
run-weather       # bazel run //services/weather:weather_main
run-tests         # bazel test //framework:framework_test
```

### 🧹 **Utility Aliases**
```bash
bazel-clean       # bazel clean
bazel-info        # bazel info
make-all          # make all
cmake-build       # mkdir build && cd build && cmake .. && make
```

## VS Code Integration

### 🎯 **Tasks** (Ctrl+Shift+P -> "Tasks: Run Task")
- **Bazel: Build All** - Build entire project
- **Bazel: Build Framework** - Build framework only
- **Bazel: Build Services** - Build services only
- **Bazel: Run Demo** - Run main demo
- **Bazel: Run REST API Demo** - Run REST API server
- **Bazel: Run Tests** - Run unit tests
- **Make: Build All** - Build with Make
- **CMake: Configure and Build** - Build with CMake

### 🐛 **Debug Configurations** (F5)
- **Debug Service Framework Demo** - Debug main demo
- **Debug REST API Demo** - Debug REST API server
- **Debug Weather Demo** - Debug weather service
- **Debug Framework Tests** - Debug unit tests

### ⚙️ **Settings**
- Bazel integration with buildifier formatting
- C++17 standard configuration
- Proper file associations for BUILD files
- IntelliSense configuration

## Port Forwarding

The devcontainer automatically forwards port **8080** for the REST API service:
- **8080** - REST API Server (labeled as "REST API Server")

When you run the REST API demo, VS Code will notify you that the port is available.

## File Structure

```
.devcontainer/
├── devcontainer.json     # Main devcontainer configuration
├── Dockerfile           # Container image definition
├── setup.sh            # Post-creation setup script
├── build_container.sh  # Container build script
└── README.md           # This file

.vscode/
├── tasks.json          # VS Code tasks for build systems
├── launch.json         # Debug configurations
└── settings.json       # Workspace settings (auto-generated)
```

## Customization

### Adding New Extensions
Edit `.devcontainer/devcontainer.json`:
```json
"extensions": [
    "existing.extension",
    "new.extension.id"
]
```

### Adding New Tools
Edit `.devcontainer/Dockerfile`:
```dockerfile
RUN apt-get update && \
    apt-get install -y your-new-tool && \
    rm -rf /var/lib/apt/lists/*
```

### Custom Aliases
Edit `.devcontainer/setup.sh` to add more aliases:
```bash
cat >> ~/.bashrc << 'EOF'
alias your-alias='your-command'
EOF
```

## Troubleshooting

### Container Build Issues
```bash
# Rebuild container
Ctrl+Shift+P -> "Dev Containers: Rebuild Container"

# Or rebuild without cache
Ctrl+Shift+P -> "Dev Containers: Rebuild Container Without Cache"
```

### Bazel Issues
```bash
# Clean Bazel cache
bazel clean --expunge

# Check Bazel version
bazel version

# Verify Bazel installation
which bazel
```

### Extension Issues
```bash
# Reload VS Code window
Ctrl+Shift+P -> "Developer: Reload Window"

# Check extension status
Ctrl+Shift+P -> "Extensions: Show Installed Extensions"
```

### Permission Issues
```bash
# Fix file permissions
sudo chown -R ubuntu:ubuntu /workspace

# Check user
whoami  # Should be 'ubuntu'
```

## Performance Tips

### 🚀 **Bazel Performance**
- Build cache is configured in `~/.bazelrc`
- Uses 75% of available RAM and CPU
- Repository cache speeds up dependency downloads

### 💾 **Storage**
- Build artifacts are cached in `~/workspace/build-cache`
- Logs are stored in `~/workspace/logs`
- Clean periodically with `bazel clean`

### 🔄 **Development Workflow**
1. Use `build-all` for full builds
2. Use specific targets for faster iteration
3. Use `run-tests` frequently during development
4. Use VS Code tasks for common operations

## Build System Comparison

| Feature | Bazel | Make | CMake |
|---------|-------|------|-------|
| **Incremental Builds** | ✅ Excellent | ✅ Good | ✅ Good |
| **Dependency Management** | ✅ Automatic | ❌ Manual | ✅ Good |
| **Scalability** | ✅ Excellent | ❌ Poor | ✅ Good |
| **IDE Integration** | ✅ Good | ✅ Excellent | ✅ Excellent |
| **Learning Curve** | ❌ Steep | ✅ Easy | ✅ Moderate |
| **Build Speed** | ✅ Fast | ✅ Fast | ✅ Moderate |

## Getting Help

- **Project Documentation**: See `README.md` and `BAZEL.md`
- **Bazel Documentation**: https://bazel.build/docs
- **VS Code Devcontainers**: https://code.visualstudio.com/docs/devcontainers/containers
- **Container Issues**: Check the VS Code Developer Console (F12)

## Contributing

When modifying the devcontainer:
1. Test changes locally
2. Update this README
3. Verify all build systems work
4. Test VS Code integration
5. Document any new features or requirements
