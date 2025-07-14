#include "../../framework/service_factory.h"
#include "../../framework/service_manager.h"
#include "../file_monitor/file_monitor_service.h"
#include "../logging/logging_service.h"
#include "weather_service.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace ServiceFramework;

int main() {
    std::cout << "Custom Service Example" << std::endl;
    std::cout << "======================" << std::endl;

    try {
        // Create service manager
        ServiceManager manager;

        // Add our custom services
        std::cout << "\n--- Adding Custom Services ---" << std::endl;
        manager.addService("WeatherService", "weather_monitor");
        manager.addService("FileMonitorService", "file_watcher");

        // Also add some built-in services
        manager.addService("LoggingService", "logger");

        // Initialize and start all services
        std::cout << "\n--- Starting Services ---" << std::endl;
        if (!manager.initializeAll() || !manager.startAll()) {
            std::cerr << "Failed to start services" << std::endl;
            return 1;
        }

        // Use the custom services
        std::cout << "\n--- Using Custom Services ---" << std::endl;

        // Get the weather service
        auto *weather = dynamic_cast<WeatherService *>(
            manager.getService("weather_monitor"));
        if (weather) {
            std::cout << "Weather Report: " << weather->getWeatherReport()
                      << std::endl;

            // Wait a bit and check again
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout << "Updated Weather: " << weather->getWeatherReport()
                      << std::endl;
        }

        // Get and use logging service
        auto *logger =
            dynamic_cast<LoggingService *>(manager.getService("logger"));
        if (logger) {
            logger->logMessage("LoggingService started successfully");
        }

        // Get the file monitor service
        // auto* fileMonitor =
        // dynamic_cast<FileMonitorService*>(manager.getService("file_watcher"));
        // if (fileMonitor) {
        //     fileMonitor->addFileToWatch("/tmp/test1.txt");
        //     fileMonitor->addFileToWatch("/tmp/test2.txt");
        //     fileMonitor->addFileToWatch("/home/user/important.doc");

        //     std::cout << "File Monitor: Watching " <<
        //     fileMonitor->getWatchedFileCount() << " files" << std::endl;
        // }

        // // Use the logging service
        // auto* logger =
        // dynamic_cast<LoggingService*>(manager.getService("logger")); if
        // (logger) {
        //     logger->logMessage("Custom services are working perfectly!");
        // }

        std::cout << "\n--- Service Information ---" << std::endl;
        std::cout << "Total services running: " << manager.getServiceCount()
                  << std::endl;
        std::cout << "Service instances:" << std::endl;
        for (const auto &name : manager.getServiceNames()) {
            auto *service = manager.getService(name);
            std::cout << "- " << name << " (" << service->getName() << ") - "
                      << (service->isRunning() ? "Running" : "Stopped")
                      << std::endl;
        }

        // Demonstrate factory features
        std::cout << "\n--- Available Service Types ---" << std::endl;
        auto &factory = ServiceFactory::getInstance();
        for (const auto &serviceName : factory.getRegisteredServices()) {
            std::cout << "- " << serviceName << std::endl;
        }

        // Clean shutdown
        std::cout << "\n--- Shutting Down ---" << std::endl;
        manager.stopAll();

        std::cout << "\nCustom service example completed successfully!"
                  << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
