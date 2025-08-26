#include "framework/service_factory.h"
#include "framework/service_manager.h"
#include "services/examples/example_services.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace ServiceFramework;

/**
 * @brief Another custom service example - File Monitor Service
 */
class FileMonitorService : public IService {
  public:
    FileMonitorService() : m_running(false), m_filesWatched(0) {}

    bool initialize() override {
        std::cout << "FileMonitorService: Initializing file monitoring..."
                  << std::endl;
        return true;
    }

    bool start() override {
        std::cout << "FileMonitorService: Starting file monitoring..."
                  << std::endl;
        m_running = true;
        return true;
    }

    void stop() override {
        std::cout << "FileMonitorService: Stopping file monitoring..."
                  << std::endl;
        m_running = false;
        m_watchedFiles.clear();
    }

    std::string getName() const override { return "FileMonitorService"; }

    bool isRunning() const override { return m_running; }

    // Custom methods
    void addFileToWatch(const std::string &filepath) {
        if (m_running) {
            m_watchedFiles.push_back(filepath);
            m_filesWatched++;
            std::cout << "FileMonitorService: Now watching " << filepath
                      << std::endl;
        }
    }

    size_t getWatchedFileCount() const { return m_filesWatched; }

  private:
    std::atomic<bool> m_running;
    std::vector<std::string> m_watchedFiles;
    std::atomic<size_t> m_filesWatched;
};
