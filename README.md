# C++ Services Framework

A flexible and extensible C++ framework for managing services using the Factory Pattern. This framework provides a clean architecture for creating, managing, and orchestrating multiple services in your application.

## Features

- **Factory Pattern Implementation**: Dynamic service creation and registration
- **Service Lifecycle Management**: Initialize, start, stop, and cleanup services
- **Multiple Service Instances**: Support for multiple instances of the same service type
- **Automatic Registration**: Macro-based service registration at compile time
- **Thread-Safe Operations**: Safe for multi-threaded environments
- **Extensible Design**: Easy to add new service types
- **Modern C++17**: Uses modern C++ features and best practices

## Architecture

The framework consists of several key components:

### Core Components

1. **IService Interface** (`service_interface.h`)
   - Base interface that all services must implement
   - Defines lifecycle methods: `initialize()`, `start()`, `stop()`
   - Provides service identification and status methods

2. **ServiceFactory** (`service_factory.h`, `service_factory.cpp`)
   - Singleton factory for creating services
   - Manages service registration and creation
   - Supports runtime service discovery

3. **ServiceManager** (`service_manager.h`, `service_manager.cpp`)
   - Manages multiple service instances
   - Handles service lifecycle orchestration
   - Maintains service dependencies and order

4. **Example Services** (`example_services.h`)
   - Demonstrates framework usage with concrete implementations
   - Includes LoggingService, DatabaseService, NetworkService, and CacheService

## Quick Start

### Building the Project

#### Using Make (Recommended for quick start)
```bash
# Build everything
make all

# Build and run demo
make run-demo

# Build and run tests
make run-test

# Build static library only
make lib

# Clean build artifacts
make clean
```

#### Using CMake
```bash
mkdir build && cd build
cmake ..
make
```

### Basic Usage

```cpp
#include "service_manager.h"
#include "example_services.h"

using namespace ServiceFramework;

int main() {
    // Create service manager
    ServiceManager manager;
    
    // Add services
    manager.addService("LoggingService", "logger");
    manager.addService("DatabaseService", "database");
    
    // Initialize and start all services
    manager.initializeAll();
    manager.startAll();
    
    // Use services
    auto* logger = dynamic_cast<LoggingService*>(manager.getService("logger"));
    if (logger) {
        logger->logMessage("Application started");
    }
    
    // Cleanup
    manager.stopAll();
    return 0;
}
```

## Creating Custom Services

### Step 1: Implement the IService Interface

```cpp
#include "service_interface.h"

class MyCustomService : public ServiceFramework::IService {
public:
    bool initialize() override {
        // Initialize your service
        std::cout << "MyCustomService: Initializing..." << std::endl;
        return true;
    }
    
    bool start() override {
        // Start your service
        std::cout << "MyCustomService: Starting..." << std::endl;
        m_running = true;
        return true;
    }
    
    void stop() override {
        // Stop your service
        std::cout << "MyCustomService: Stopping..." << std::endl;
        m_running = false;
    }
    
    std::string getName() const override {
        return "MyCustomService";
    }
    
    bool isRunning() const override {
        return m_running;
    }
    
    // Add your custom methods here
    void doSomething() {
        if (m_running) {
            std::cout << "MyCustomService: Doing something..." << std::endl;
        }
    }

private:
    std::atomic<bool> m_running{false};
};
```

### Step 2: Register the Service

```cpp
// Register the service for automatic factory creation
REGISTER_SERVICE(MyCustomService, "MyCustomService");
```

### Step 3: Use the Service

```cpp
ServiceManager manager;
manager.addService("MyCustomService", "my_service");
manager.initializeAll();
manager.startAll();

auto* myService = dynamic_cast<MyCustomService*>(manager.getService("my_service"));
if (myService) {
    myService->doSomething();
}
```

## Advanced Usage

### Multiple Service Instances

```cpp
ServiceManager manager;

// Create multiple instances of the same service type
manager.addService("DatabaseService", "primary_db");
manager.addService("DatabaseService", "secondary_db");
manager.addService("CacheService", "redis_cache");
manager.addService("CacheService", "memory_cache");

manager.initializeAll();
manager.startAll();

// Use different instances independently
auto* primaryDb = dynamic_cast<DatabaseService*>(manager.getService("primary_db"));
auto* secondaryDb = dynamic_cast<DatabaseService*>(manager.getService("secondary_db"));
```

### Manual Service Creation

```cpp
auto& factory = ServiceFactory::getInstance();

// Create service directly from factory
auto service = factory.createService("LoggingService");
if (service) {
    service->initialize();
    service->start();
    // Use service...
    service->stop();
}
```

### Service Discovery

```cpp
auto& factory = ServiceFactory::getInstance();

// List all registered services
auto services = factory.getRegisteredServices();
for (const auto& serviceName : services) {
    std::cout << "Available service: " << serviceName << std::endl;
}

// Check if specific service is available
if (factory.isServiceRegistered("MyCustomService")) {
    auto service = factory.createService("MyCustomService");
    // Use service...
}
```

## Example Services

The framework includes several example services to demonstrate usage:

### LoggingService
- Provides application logging functionality
- Thread-safe message logging
- Configurable log levels and outputs

### DatabaseService
- Simulates database connectivity and operations
- Connection management
- Query execution interface

### NetworkService
- Network server simulation
- Configurable port settings
- Connection handling

### CacheService
- In-memory key-value cache
- Thread-safe operations
- Configurable cache policies

## Thread Safety

The framework is designed to be thread-safe:
- ServiceFactory uses singleton pattern with thread-safe initialization
- ServiceManager operations are protected for concurrent access
- Individual services should implement their own thread safety as needed

## Best Practices

1. **Service Dependencies**: Initialize services in dependency order
2. **Error Handling**: Always check return values from `initialize()` and `start()`
3. **Resource Cleanup**: Ensure `stop()` properly cleans up resources
4. **Thread Safety**: Implement appropriate synchronization in custom services
5. **Service Naming**: Use descriptive names for service instances
6. **Lifecycle Management**: Follow the initialize → start → stop → cleanup pattern

## Testing

Run the included tests to verify framework functionality:

```bash
make run-test
```

The test suite covers:
- Service factory operations
- Service manager functionality
- Multiple service instances
- Error handling scenarios
- Service lifecycle management

## Contributing

When adding new features:
1. Follow the existing code style and patterns
2. Add appropriate tests for new functionality
3. Update documentation and examples
4. Ensure thread safety where applicable

## License

This framework is provided as-is for educational and development purposes. Feel free to modify and extend it for your specific needs.
