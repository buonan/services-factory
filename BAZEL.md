# Bazel Build Guide for C++ Services Framework

This document provides detailed instructions for building and using the C++ Services Framework with Bazel.

## Prerequisites

- Bazel 8.0+ installed
- C++17 compatible compiler (GCC 7+ or Clang 5+)
- Linux/macOS environment

## Installation

### Installing Bazel

#### Ubuntu/Debian
```bash
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg
sudo mv bazel-archive-keyring.gpg /usr/share/keyrings
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
sudo apt update && sudo apt install bazel
```

#### macOS
```bash
brew install bazel
```

## Quick Start

### Build Everything
```bash
# Build all targets
bazel build //...

# Or use the convenience script
./build_bazel.sh
```

### Run Demos
```bash
# Main framework demo
bazel run //:service_framework_demo

# REST API server demo
bazel run //services/rest_api:rest_api_demo

# Weather service demo
bazel run //services/weather:weather_main

# REST API test client (run in separate terminal after starting REST API demo)
bazel run //services/rest_api:test_client
```

### Run Tests
```bash
# Run framework tests
bazel test //framework:framework_test

# Run all tests
bazel test //...
```

## Project Structure

The Bazel build is organized as follows:

```
/
├── WORKSPACE              # Legacy workspace file (kept for compatibility)
├── MODULE.bazel          # Modern Bazel module definition
├── .bazelrc              # Bazel configuration
├── BUILD                 # Root build file
├── framework/
│   └── BUILD             # Framework core components
├── services/
│   ├── BUILD             # Services aggregation
│   ├── cache/BUILD       # Cache service
│   ├── database/BUILD    # Database service
│   ├── examples/BUILD    # Example services
│   ├── file_monitor/BUILD # File monitor service
│   ├── logging/BUILD     # Logging service
│   ├── network/BUILD     # Network service
│   ├── rest_api/BUILD    # REST API service
│   └── weather/BUILD     # Weather service
```

## Build Targets

### Core Framework
```bash
# Build framework core
bazel build //framework:framework_core

# Build individual components
bazel build //framework:service_interface
bazel build //framework:service_factory
bazel build //framework:service_manager
```

### Services
```bash
# Build all services
bazel build //services:all_services

# Build individual services
bazel build //services/logging:logging_service
bazel build //services/rest_api:rest_api_service
bazel build //services/weather:weather_service
```

### Executables
```bash
# Main demo
bazel build //:service_framework_demo

# REST API demos
bazel build //services/rest_api:rest_api_demo
bazel build //services/rest_api:test_client

# Weather service demo
bazel build //services/weather:weather_main
```

## Configuration

### Build Configurations

The project supports different build configurations via `.bazelrc`:

```bash
# Debug build
bazel build --config=debug //...

# Release build (default)
bazel build --config=release //...
```

### Compiler Flags

Default flags are set in `.bazelrc`:
- C++17 standard
- Wall, Wextra, Wpedantic warnings
- Threading support (-lpthread)

## Advanced Usage

### Custom Build Flags
```bash
# Build with specific compiler
bazel build --cxxopt=-std=c++20 //...

# Build with additional warnings
bazel build --cxxopt=-Werror //...

# Verbose build output
bazel build --verbose_failures //...
```

### Dependency Analysis
```bash
# Show dependency graph
bazel query --output=graph //... | dot -Tpng > deps.png

# List all targets
bazel query //...

# Show dependencies of a target
bazel query "deps(//framework:framework_core)"
```

### Performance
```bash
# Parallel builds (adjust number based on CPU cores)
bazel build --jobs=8 //...

# Use remote cache (if available)
bazel build --remote_cache=... //...
```

## Troubleshooting

### Common Issues

1. **Include Path Issues**
   - The project uses `strip_include_prefix` and `include_prefix` to organize headers
   - Use `#include "framework/service_interface.h"` instead of relative paths

2. **Service Registration**
   - Some services may not register automatically in Bazel builds
   - This is expected and doesn't affect core functionality

3. **Build Cache Issues**
   ```bash
   # Clean build cache
   bazel clean
   
   # Clean everything including external dependencies
   bazel clean --expunge
   ```

4. **Dependency Issues**
   ```bash
   # Update external dependencies
   bazel sync
   
   # Show external dependencies
   bazel query @...
   ```

### Debug Build Issues
```bash
# Verbose output
bazel build --verbose_failures --sandbox_debug //...

# Keep sandbox for debugging
bazel build --sandbox_debug --sandbox_writable_path=/tmp //...
```

## Integration with IDEs

### VS Code
Add to `.vscode/settings.json`:
```json
{
    "bazel.buildifierFixOnFormat": true,
    "bazel.queriesShareServer": true
}
```

### CLion
1. Install Bazel plugin
2. Import project as Bazel project
3. Configure build targets in run configurations

## Comparison with Other Build Systems

| Feature | Bazel | Make | CMake |
|---------|-------|------|-------|
| Incremental builds | ✅ Excellent | ✅ Good | ✅ Good |
| Dependency management | ✅ Excellent | ❌ Manual | ✅ Good |
| Cross-platform | ✅ Excellent | ❌ Limited | ✅ Excellent |
| Large project scaling | ✅ Excellent | ❌ Poor | ✅ Good |
| Learning curve | ❌ Steep | ✅ Easy | ✅ Moderate |
| External dependencies | ✅ Excellent | ❌ Manual | ✅ Good |

## Best Practices

1. **Target Granularity**: Keep targets focused and small
2. **Visibility**: Use appropriate visibility declarations
3. **Dependencies**: Minimize dependencies between packages
4. **Headers**: Use proper include prefixes for clean header organization
5. **Testing**: Write tests for each component
6. **Documentation**: Document complex BUILD rules

## Contributing

When adding new components:

1. Create appropriate BUILD files
2. Use consistent naming conventions
3. Add proper dependencies
4. Include tests where applicable
5. Update this documentation

## Resources

- [Bazel Documentation](https://bazel.build/docs)
- [C++ Rules Documentation](https://github.com/bazelbuild/rules_cc)
- [Bazel Best Practices](https://bazel.build/concepts/best-practices)
