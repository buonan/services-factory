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

/**
 * @brief Example cache service
 */
class CacheService : public IService {
  public:
    CacheService() : m_running(false) {}

    bool initialize() override {
        std::cout << "CacheService: Initializing cache..." << std::endl;
        // Initialize cache storage
        return true;
    }

    bool start() override {
        std::cout << "CacheService: Starting cache service..." << std::endl;
        m_running = true;
        return true;
    }

    void stop() override {
        std::cout << "CacheService: Stopping cache service..." << std::endl;
        m_cache.clear();
        m_running = false;
    }

    std::string getName() const override { return "CacheService"; }

    bool isRunning() const override { return m_running; }

    // Service-specific methods
    void set(const std::string &key, const std::string &value) {
        if (m_running) {
            m_cache[key] = value;
            std::cout << "[CACHE] Set: " << key << " = " << value << std::endl;
        }
    }

    std::string get(const std::string &key) {
        if (m_running) {
            auto it = m_cache.find(key);
            if (it != m_cache.end()) {
                std::cout << "[CACHE] Get: " << key << " = " << it->second
                          << std::endl;
                return it->second;
            }
        }
        return "";
    }

  private:
    std::atomic<bool> m_running;
    std::unordered_map<std::string, std::string> m_cache;
};

} // namespace ServiceFramework

// Register all example services using a different approach
// namespace {
//     struct ServiceRegistrations {
//         ServiceRegistrations() {
//             auto& factory = ServiceFramework::ServiceFactory::getInstance();

//             factory.registerService("LoggingService", []() ->
//             ServiceFramework::ServicePtr {
//                 return std::make_unique<ServiceFramework::LoggingService>();
//             });

//             factory.registerService("DatabaseService", []() ->
//             ServiceFramework::ServicePtr {
//                 return std::make_unique<ServiceFramework::DatabaseService>();
//             });

//             factory.registerService("NetworkService", []() ->
//             ServiceFramework::ServicePtr {
//                 return std::make_unique<ServiceFramework::NetworkService>();
//             });

//             factory.registerService("CacheService", []() ->
//             ServiceFramework::ServicePtr {
//                 return std::make_unique<ServiceFramework::CacheService>();
//             });
//         }
//     };

//     static ServiceRegistrations g_serviceRegistrations;
// }
