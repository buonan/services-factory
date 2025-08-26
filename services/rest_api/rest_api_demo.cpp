#include "rest_api_service.h"
#include "rest_api_registration.h"
#include "../../framework/service_manager.h"
#include "../logging/logging_service.h"
#include "../database/database.h"
#include "../cache/cache.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace ServiceFramework;

// Register missing services
namespace {
struct AdditionalServiceRegistrations {
    AdditionalServiceRegistrations() {
        auto &factory = ServiceFramework::ServiceFactory::getInstance();

        factory.registerService(
            "DatabaseService", []() -> ServiceFramework::ServicePtr {
                return std::make_unique<ServiceFramework::DatabaseService>();
            });
            
        factory.registerService(
            "CacheService", []() -> ServiceFramework::ServicePtr {
                return std::make_unique<ServiceFramework::CacheService>();
            });
    }
};

static AdditionalServiceRegistrations g_additionalServiceRegistrations;
} // namespace

int main() {
    std::cout << "=== REST API Service Demo ===" << std::endl;
    
    try {
        // Create service manager
        ServiceManager manager;
        
        // Add various services
        std::cout << "\n1. Adding services..." << std::endl;
        manager.addService("LoggingService", "logger");
        manager.addService("DatabaseService", "primary_db");
        manager.addService("DatabaseService", "secondary_db");
        manager.addService("CacheService", "redis_cache");
        manager.addService("RestApiService", "api_server");
        
        // Get the REST API service and configure it
        auto* apiService = dynamic_cast<RestApiService*>(manager.getService("api_server"));
        if (apiService) {
            apiService->setServiceManager(&manager);
            apiService->setPort(8080);
            
            // Add custom routes
            apiService->addRoute("GET", "/api/custom/hello", [](const HttpRequest& req) {
                HttpResponse response;
                response.body = R"({"message": "Hello from custom endpoint!", "timestamp": ")" + 
                               std::to_string(std::time(nullptr)) + R"("})";
                return response;
            });
            
            apiService->addRoute("POST", "/api/custom/echo", [](const HttpRequest& req) {
                HttpResponse response;
                response.body = R"({"echo": ")" + req.body + R"(", "method": ")" + req.method + R"("})";
                return response;
            });
        }
        
        // Initialize all services
        std::cout << "\n2. Initializing services..." << std::endl;
        if (!manager.initializeAll()) {
            std::cerr << "Failed to initialize services" << std::endl;
            return 1;
        }
        
        // Start all services
        std::cout << "\n3. Starting services..." << std::endl;
        if (!manager.startAll()) {
            std::cerr << "Failed to start services" << std::endl;
            return 1;
        }
        
        std::cout << "\n=== REST API Server is now running ===" << std::endl;
        std::cout << "Server is listening on http://localhost:8080" << std::endl;
        std::cout << "\nAvailable endpoints:" << std::endl;
        std::cout << "  GET    http://localhost:8080/api/services" << std::endl;
        std::cout << "  GET    http://localhost:8080/api/services/{name}" << std::endl;
        std::cout << "  GET    http://localhost:8080/api/health/{name}" << std::endl;
        std::cout << "  POST   http://localhost:8080/api/services/{name}/start" << std::endl;
        std::cout << "  POST   http://localhost:8080/api/services/{name}/stop" << std::endl;
        std::cout << "  GET    http://localhost:8080/api/status" << std::endl;
        std::cout << "  GET    http://localhost:8080/api/custom/hello" << std::endl;
        std::cout << "  POST   http://localhost:8080/api/custom/echo" << std::endl;
        
        std::cout << "\nExample curl commands:" << std::endl;
        std::cout << "  curl http://localhost:8080/api/services" << std::endl;
        std::cout << "  curl http://localhost:8080/api/services/logger" << std::endl;
        std::cout << "  curl http://localhost:8080/api/health/primary_db" << std::endl;
        std::cout << "  curl -X POST http://localhost:8080/api/services/logger/stop" << std::endl;
        std::cout << "  curl http://localhost:8080/api/custom/hello" << std::endl;
        std::cout << "  curl -X POST -d 'Hello World' http://localhost:8080/api/custom/echo" << std::endl;
        
        std::cout << "\nPress Ctrl+C to stop the server..." << std::endl;
        
        // Keep the server running
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // Check if API service is still running
            if (!apiService || !apiService->isRunning()) {
                std::cout << "API service stopped unexpectedly" << std::endl;
                break;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nShutting down..." << std::endl;
    return 0;
}
