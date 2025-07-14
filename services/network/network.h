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
 * @brief Example network service
 */
class NetworkService : public IService {
  public:
    NetworkService() : m_running(false), m_port(8080) {}

    bool initialize() override {
        std::cout << "NetworkService: Initializing on port " << m_port << "..."
                  << std::endl;
        // Initialize network components
        return true;
    }

    bool start() override {
        std::cout << "NetworkService: Starting server..." << std::endl;
        m_running = true;
        // Start network server
        return true;
    }

    void stop() override {
        std::cout << "NetworkService: Stopping server..." << std::endl;
        m_running = false;
    }

    std::string getName() const override { return "NetworkService"; }

    bool isRunning() const override { return m_running; }

    // Service-specific methods
    void setPort(int port) {
        if (!m_running) {
            m_port = port;
        }
    }

    int getPort() const { return m_port; }

  private:
    std::atomic<bool> m_running;
    int m_port;
};

} // namespace ServiceFramework
