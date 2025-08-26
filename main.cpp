#include "framework/service_factory.h"
#include "framework/service_manager.h"
#include "services/examples/example_services.h"
#include "services/logging/logging_service.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace ServiceFramework;

void demonstrateBasicUsage() {
    std::cout << "\n=== Basic Service Framework Usage ===" << std::endl;

    // Create service manager
    ServiceManager manager;

    // Add services using the factory
    std::cout << "\n--- Adding Services ---" << std::endl;
    manager.addService("LoggingService", "logger");
    manager.addService("DatabaseService", "maindb");
    manager.addService("NetworkService", "webserver");
    manager.addService("CacheService", "cache");

    // Initialize all services
    std::cout << "\n--- Initializing Services ---" << std::endl;
    if (!manager.initializeAll()) {
        std::cerr << "Failed to initialize services" << std::endl;
        return;
    }

    // Start all services
    std::cout << "\n--- Starting Services ---" << std::endl;
    if (!manager.startAll()) {
        std::cerr << "Failed to start services" << std::endl;
        return;
    }

    // Use the services
    std::cout << "\n--- Using Services ---" << std::endl;

    // Get and use logging service
    auto *logger = dynamic_cast<LoggingService *>(manager.getService("logger"));
    if (logger) {
        logger->logMessage("Application started successfully");
    }

    // Get and use database service
    auto *database =
        dynamic_cast<DatabaseService *>(manager.getService("maindb"));
    if (database) {
        database->executeQuery("SELECT * FROM users");
    }

    // Get and use cache service
    auto *cache = dynamic_cast<CacheService *>(manager.getService("cache"));
    if (cache) {
        cache->set("user:123", "John Doe");
        std::string user = cache->get("user:123");
    }

    // Get and use network service
    auto *network =
        dynamic_cast<NetworkService *>(manager.getService("webserver"));
    if (network) {
        std::cout << "Network service running on port: " << network->getPort()
                  << std::endl;
    }

    // Simulate some work
    std::cout << "\n--- Simulating Work ---" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (logger) {
        logger->logMessage("Work completed");
    }

    // Stop all services
    std::cout << "\n--- Stopping Services ---" << std::endl;
    manager.stopAll();
}

void demonstrateFactoryFeatures() {
    std::cout << "\n=== Service Factory Features ===" << std::endl;

    auto &factory = ServiceFactory::getInstance();

    // List registered services
    std::cout << "\n--- Registered Services ---" << std::endl;
    auto services = factory.getRegisteredServices();
    for (const auto &service : services) {
        std::cout << "- " << service << std::endl;
    }

    // Create individual services
    std::cout << "\n--- Creating Individual Services ---" << std::endl;
    auto loggingService = factory.createService("LoggingService");
    if (loggingService) {
        loggingService->initialize();
        loggingService->start();

        // Use the service
        auto *logger = dynamic_cast<LoggingService *>(loggingService.get());
        if (logger) {
            logger->logMessage("Individual service test");
        }

        loggingService->stop();
    }
}

void demonstrateServiceManager() {
    std::cout << "\n=== Service Manager Features ===" << std::endl;

    ServiceManager manager;

    // Add services with custom instance names
    std::cout << "\n--- Adding Services with Custom Names ---" << std::endl;
    manager.addService("DatabaseService", "primary_db");
    manager.addService("DatabaseService", "secondary_db");
    manager.addService("CacheService", "redis_cache");
    manager.addService("CacheService", "memory_cache");
    manager.addService("WeatherService", "weather_monitor");

    // Show service information
    std::cout << "\nService count: " << manager.getServiceCount() << std::endl;
    std::cout << "Service instances:" << std::endl;
    for (const auto &name : manager.getServiceNames()) {
        std::cout << "- " << name << std::endl;
    }

    // Initialize and start services
    manager.initializeAll();
    manager.startAll();

    // Use multiple instances of the same service type
    std::cout << "\n--- Using Multiple Service Instances ---" << std::endl;
    auto *primaryDb =
        dynamic_cast<DatabaseService *>(manager.getService("primary_db"));
    auto *secondaryDb =
        dynamic_cast<DatabaseService *>(manager.getService("secondary_db"));

    if (primaryDb) {
        primaryDb->executeQuery("INSERT INTO users VALUES (1, 'Alice')");
    }

    if (secondaryDb) {
        secondaryDb->executeQuery("SELECT * FROM backup_users");
    }

    // Remove a specific service
    std::cout << "\n--- Removing Service ---" << std::endl;
    manager.removeService("secondary_db");
    std::cout << "Service count after removal: " << manager.getServiceCount()
              << std::endl;

    // Clean up
    manager.clear();
}

int main() {
    std::cout << "C++ Services Framework Demo" << std::endl;
    std::cout << "===========================" << std::endl;

    try {
        // Demonstrate basic usage
        demonstrateBasicUsage();

        // Demonstrate factory features
        demonstrateFactoryFeatures();

        // Demonstrate service manager features
        demonstrateServiceManager();

        std::cout << "\n=== Demo Completed Successfully ===" << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
