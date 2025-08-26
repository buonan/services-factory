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
 * @brief Example database service
 */
class DatabaseService : public IService {
  public:
    DatabaseService() : m_running(false), m_connected(false) {}

    bool initialize() override {
        std::cout << "DatabaseService: Initializing..." << std::endl;
        // Initialize database connection
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)); // Simulate init time
        return true;
    }

    bool health() override { 
        return m_running && m_connected; 
    }

    bool start() override {
        std::cout << "DatabaseService: Starting..." << std::endl;
        m_connected = true;
        m_running = true;
        return true;
    }

    void stop() override {
        std::cout << "DatabaseService: Stopping..." << std::endl;
        m_connected = false;
        m_running = false;
    }

    std::string getName() const override { return "DatabaseService"; }

    bool isRunning() const override { return m_running; }

    // Service-specific methods
    bool executeQuery(const std::string &query) {
        if (m_connected) {
            std::cout << "[DB] Executing query: " << query << std::endl;
            return true;
        }
        return false;
    }

  private:
    std::atomic<bool> m_running;
    std::atomic<bool> m_connected;
};

} // namespace ServiceFramework
