# REST API Service

A comprehensive RESTful API service for the C++ Services Framework that provides HTTP endpoints to manage and interact with other services in the framework.

## Features

- **HTTP Server**: Built-in lightweight HTTP server (no external dependencies)
- **Service Management**: REST endpoints to control other services
- **Thread Pool**: Multi-threaded request handling for concurrent connections
- **Custom Routes**: Easy addition of custom API endpoints
- **JSON Responses**: Structured JSON responses for all endpoints
- **Error Handling**: Proper HTTP status codes and error messages
- **Path Parameters**: Support for parameterized routes (e.g., `/api/services/{name}`)
- **Query Parameters**: URL query parameter parsing
- **Request Body**: Support for POST/PUT request bodies

## Architecture

The REST API service consists of several key components:

### Core Components

1. **RestApiService**: Main service class implementing IService interface
2. **HttpRequest/HttpResponse**: Structures for HTTP message handling
3. **RouteHandler**: Function type for handling HTTP requests
4. **Thread Pool**: Worker threads for handling concurrent requests

### Built-in Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/services` | List all services |
| GET | `/api/services/{name}` | Get service information |
| GET | `/api/health/{name}` | Check service health |
| POST | `/api/services/{name}/start` | Start a service |
| POST | `/api/services/{name}/stop` | Stop a service |
| GET | `/api/status` | Get API server status |

## Quick Start

### Building

```bash
# Using Makefile
make run-rest-api

# Using CMake
mkdir build && cd build
cmake ..
make
./RestApiDemo
```

### Basic Usage

```cpp
#include "services/rest_api/rest_api_service.h"
#include "services/rest_api/rest_api_registration.h"
#include "framework/service_manager.h"

using namespace ServiceFramework;

int main() {
    ServiceManager manager;
    
    // Add services
    manager.addService("LoggingService", "logger");
    manager.addService("DatabaseService", "database");
    manager.addService("RestApiService", "api_server");
    
    // Configure REST API service
    auto* apiService = dynamic_cast<RestApiService*>(manager.getService("api_server"));
    if (apiService) {
        apiService->setServiceManager(&manager);
        apiService->setPort(8080);
    }
    
    // Initialize and start all services
    manager.initializeAll();
    manager.startAll();
    
    // Server is now running on http://localhost:8080
    // Keep running...
    
    return 0;
}
```

## API Reference

### Service Management Endpoints

#### List All Services
```http
GET /api/services
```

**Response:**
```json
{
  "services": [
    {
      "name": "logger",
      "type": "LoggingService",
      "running": true
    },
    {
      "name": "database",
      "type": "DatabaseService", 
      "running": false
    }
  ]
}
```

#### Get Service Information
```http
GET /api/services/{name}
```

**Response:**
```json
{
  "name": "logger",
  "type": "LoggingService",
  "running": true,
  "healthy": true
}
```

#### Check Service Health
```http
GET /api/health/{name}
```

**Response:**
```json
{
  "healthy": true
}
```

#### Start Service
```http
POST /api/services/{name}/start
```

**Response:**
```json
{
  "started": true
}
```

#### Stop Service
```http
POST /api/services/{name}/stop
```

**Response:**
```json
{
  "stopped": true
}
```

#### API Server Status
```http
GET /api/status
```

**Response:**
```json
{
  "service": "RestApiService",
  "status": "running",
  "port": 8080,
  "endpoints": [
    "GET /api/services",
    "GET /api/services/{name}",
    "GET /api/health/{name}",
    "POST /api/services/{name}/start",
    "POST /api/services/{name}/stop",
    "GET /api/status"
  ]
}
```

## Custom Routes

You can easily add custom routes to extend the API functionality:

```cpp
auto* apiService = dynamic_cast<RestApiService*>(manager.getService("api_server"));
if (apiService) {
    // Add a custom GET endpoint
    apiService->addRoute("GET", "/api/custom/hello", [](const HttpRequest& req) {
        HttpResponse response;
        response.body = R"({"message": "Hello World!", "timestamp": ")" + 
                       std::to_string(std::time(nullptr)) + R"("})";
        return response;
    });
    
    // Add a custom POST endpoint with body processing
    apiService->addRoute("POST", "/api/custom/echo", [](const HttpRequest& req) {
        HttpResponse response;
        response.body = R"({"echo": ")" + req.body + R"(", "method": ")" + req.method + R"("})";
        return response;
    });
    
    // Add parameterized route
    apiService->addRoute("GET", "/api/users/{id}", [](const HttpRequest& req) {
        HttpResponse response;
        auto idIt = req.pathParams.find("id");
        if (idIt != req.pathParams.end()) {
            response.body = R"({"user_id": ")" + idIt->second + R"(", "name": "User )" + idIt->second + R"("})";
        } else {
            response.statusCode = 400;
            response.body = R"({"error": "User ID not provided"})";
        }
        return response;
    });
}
```

## Testing

### Using curl

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

# Custom endpoints
curl http://localhost:8080/api/custom/hello
curl -X POST -d '{"test": "data"}' http://localhost:8080/api/custom/echo
```

### Using the Test Client

```bash
# Build and run the test client
make run-test-client

# Or using CMake
./build/RestApiTestClient
```

## Configuration

### Port Configuration

```cpp
auto* apiService = dynamic_cast<RestApiService*>(manager.getService("api_server"));
if (apiService) {
    apiService->setPort(9090);  // Change port before starting
}
```

### Thread Pool Size

The service uses a configurable thread pool (default: 10 worker threads). You can modify the `MAX_WORKER_THREADS` constant in the header file.

## Error Handling

The API returns appropriate HTTP status codes:

- **200 OK**: Successful request
- **400 Bad Request**: Invalid request parameters
- **404 Not Found**: Service or endpoint not found
- **405 Method Not Allowed**: HTTP method not supported for endpoint
- **500 Internal Server Error**: Service operation failed
- **503 Service Unavailable**: Service manager not available

## Security Considerations

This is a basic HTTP server implementation intended for internal service management. For production use, consider:

- Adding authentication/authorization
- Implementing HTTPS/TLS
- Rate limiting
- Input validation and sanitization
- CORS headers if needed

## Performance

- Multi-threaded request handling
- Connection pooling with worker threads
- Non-blocking socket operations
- Efficient request parsing and routing

## Limitations

- HTTP/1.1 only (no HTTP/2 support)
- No built-in authentication
- No HTTPS/TLS support
- Basic request body size limits
- No file upload support

## Examples

See the following files for complete examples:
- `rest_api_demo.cpp`: Complete demo application
- `test_client.cpp`: Test client implementation
- `rest_api_service.h/cpp`: Full service implementation
