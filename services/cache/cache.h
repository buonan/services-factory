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

    bool health() override { 
        return m_running; 
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
