#pragma once
#include "../../framework/service_factory.h"
#include "../../framework/service_interface.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <unordered_map>

namespace ServiceFramework {

/**
 * @brief Example logging service
 */
class LoggingService : public IService {
  public:
    LoggingService() : m_running(false) {}

    bool initialize() override {
        std::cout << "LoggingService: Initializing..." << std::endl;
        // Initialize logging system
        return true;
    }

    bool health() override { return true; }

    bool start() override {
        std::cout << "LoggingService: Starting..." << std::endl;
        m_running = true;
        return true;
    }

    void stop() override {
        std::cout << "LoggingService: Stopping..." << std::endl;
        m_running = false;
    }

    std::string getName() const override { return "LoggingService"; }

    bool isRunning() const override { return m_running; }

    // Service-specific methods
    void logMessage(const std::string &message) {
        if (m_running) {
            std::cout << "[LOG] " << message << std::endl;
        }
    }

  private:
    std::atomic<bool> m_running;
};

} // namespace ServiceFramework

namespace {
struct ServiceRegistrations {
    ServiceRegistrations() {
        auto &factory = ServiceFramework::ServiceFactory::getInstance();

        factory.registerService(
            "LoggingService", []() -> ServiceFramework::ServicePtr {
                return std::make_unique<ServiceFramework::LoggingService>();
            });
    }
};

static ServiceRegistrations g_serviceRegistrations;
} // namespace
