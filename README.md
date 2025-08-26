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
- **REST API Service**: Built-in HTTP REST API for service management

## Architecture

The framework consists of several key components:

### Core Components

1. **IService Interface** (`service_interface.h`)
   - Base interface that all services must implement
   - Defines lifecycle methods: `initialize()`, `start()`, `stop()`, `health()`
   - Provides service identification and status methods

2. **ServiceFactory** (`service_factory.h`, `service_factory.cpp`)
   - Singleton factory for creating services
   - Manages service registration and creation
   - Supports runtime service discovery

3. **ServiceManager** (`service_manager.h`, `service_manager.cpp`)
   - Manages multiple service instances
   - Handles service lifecycle orchestration
   - Maintains service dependencies and order

4. **REST API Service** (`services/rest_api/`)
   - HTTP REST API for service management
   - Built-in endpoints for service control
   - Support for custom routes and handlers
   - Multi-threaded request processing

5. **Example Services** (`services/`)
   - Demonstrates framework usage with concrete implementations
   - Includes LoggingService, DatabaseService, CacheService, and more

## Quick Start

### Development Environment

#### Using Dev Container (Recommended for development)
The easiest way to get started is using the provided dev container with VS Code:

1. **Prerequisites**: Install VS Code and the Dev Containers extension
2. **Open Project**: Open the project folder in VS Code
3. **Reopen in Container**: Use Command Palette (Ctrl+Shift+P) → "Dev Containers: Reopen in Container"
4. **Wait for Setup**: The container will automatically install Bazel, Make, CMake, and all dependencies
5. **Start Building**: Use the provided aliases or VS Code tasks

The dev container includes:
- ✅ Bazel 8.x with completion and VS Code integration
- ✅ Make and CMake build systems
- ✅ C++17 compiler and debugging tools
- ✅ Pre-configured VS Code extensions and tasks
- ✅ Shell aliases for common commands

See [.devcontainer/README.md](.devcontainer/README.md) for detailed setup and usage instructions.

### Building the Project

#### Using Make (Recommended for quick start)
```bash
# Build everything
make all

# Build and run main demo
make run-demo

# Build and run REST API server
make run-rest-api

# Build and run REST API test client
make run-test-client

# Build and run tests
make run-test

# Build static library only
make lib

# Clean build artifacts
make clean
```

#### Using Bazel (Recommended for large projects)
```bash
# Build everything
bazel build //...

# Build and run main demo
bazel run //:service_framework_demo

# Build and run REST API server
bazel run //:rest_api_demo

# Build and run REST API test client (in another terminal)
bazel run //:rest_api_test_client

# Build and run weather service demo
bazel run //services/weather:weather_main

# Run tests
bazel test //framework:framework_test

# Build specific components
bazel build //framework:framework_core
bazel build //services:all_services

# Use convenience script
./build_bazel.sh
```

#### Using CMake
```bash
mkdir build && cd build
cmake ..
make

# Run REST API demo
./RestApiDemo

# Run test client (in another terminal)
./RestApiTestClient
```

### Basic Usage

```cpp
#include "service_manager.h"
#include "services/logging/logging_service.h"

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

### REST API Usage

```cpp
#include "services/rest_api/rest_api_service.h"
#include "services/rest_api/rest_api_registration.h"

int main() {
    ServiceManager manager;
    
    // Add services including REST API
    manager.addService("LoggingService", "logger");
    manager.addService("RestApiService", "api_server");
    
    // Configure REST API
    auto* apiService = dynamic_cast<RestApiService*>(manager.getService("api_server"));
    if (apiService) {
        apiService->setServiceManager(&manager);
        apiService->setPort(8080);
        
        // Add custom route
        apiService->addRoute("GET", "/api/custom/hello", [](const HttpRequest& req) {
            HttpResponse response;
            response.body = R"({"message": "Hello World!"})";
            return response;
        });
    }
    
    manager.initializeAll();
    manager.startAll();
    
    // REST API server is now running on http://localhost:8080
    // Available endpoints:
    // GET /api/services - List all services
    // GET /api/services/{name} - Get service info
    // GET /api/health/{name} - Check service health
    // POST /api/services/{name}/start - Start service
    // POST /api/services/{name}/stop - Stop service
    
    return 0;
}
```

## REST API Endpoints

The REST API service provides the following built-in endpoints:

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/services` | List all services |
| GET | `/api/services/{name}` | Get service information |
| GET | `/api/health/{name}` | Check service health |
| POST | `/api/services/{name}/start` | Start a service |
| POST | `/api/services/{name}/stop` | Stop a service |
| GET | `/api/status` | Get API server status |

### Example API Calls

```bash
# List all services
curl http://localhost:8080/api/services

# Get specific service info
curl http://localhost:8080/api/services/logger

# Check service health
curl http://localhost:8080/api/health/logger

# Start a service
curl -X POST http://localhost:8080/api/services/logger/start

# Stop a service
curl -X POST http://localhost:8080/api/services/logger/stop
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
    
    bool health() override {
        return m_running;
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

### Custom REST API Routes

```cpp
auto* apiService = dynamic_cast<RestApiService*>(manager.getService("api_server"));
if (apiService) {
    // Add parameterized route
    apiService->addRoute("GET", "/api/users/{id}", [](const HttpRequest& req) {
        HttpResponse response;
        auto idIt = req.pathParams.find("id");
        if (idIt != req.pathParams.end()) {
            response.body = R"({"user_id": ")" + idIt->second + R"("})";
        }
        return response;
    });
    
    // Add POST endpoint with body processing
    apiService->addRoute("POST", "/api/data", [](const HttpRequest& req) {
        HttpResponse response;
        // Process req.body
        response.body = R"({"received": ")" + req.body + R"("})";
        return response;
    });
}
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

### CacheService
- In-memory key-value cache
- Thread-safe operations
- Configurable cache policies

### RestApiService
- HTTP REST API server
- Service management endpoints
- Custom route support
- Multi-threaded request handling

## Thread Safety

The framework is designed to be thread-safe:
- ServiceFactory uses singleton pattern with thread-safe initialization
- ServiceManager operations are protected for concurrent access
- RestApiService uses thread pool for concurrent request handling
- Individual services should implement their own thread safety as needed

## Best Practices

1. **Service Dependencies**: Initialize services in dependency order
2. **Error Handling**: Always check return values from `initialize()` and `start()`
3. **Resource Cleanup**: Ensure `stop()` properly cleans up resources
4. **Thread Safety**: Implement appropriate synchronization in custom services
5. **Service Naming**: Use descriptive names for service instances
6. **Lifecycle Management**: Follow the initialize → start → stop → cleanup pattern
7. **Health Checks**: Implement meaningful health checks in your services
8. **REST API Security**: Add authentication for production REST API usage

## Testing

Run the included tests to verify framework functionality:

```bash
# Test framework
make run-test

# Test REST API
make run-rest-api    # In one terminal
make run-test-client # In another terminal
```

The test suite covers:
- Service factory operations
- Service manager functionality
- Multiple service instances
- Error handling scenarios
- Service lifecycle management
- REST API endpoints and functionality

## Contributing

When adding new features:
1. Follow the existing code style and patterns
2. Add appropriate tests for new functionality
3. Update documentation and examples
4. Ensure thread safety where applicable
5. Add REST API endpoints for new service management features

## License

This framework is provided as-is for educational and development purposes. Feel free to modify and extend it for your specific needs.
