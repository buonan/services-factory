#include "../../framework/service_factory.h"
#include "../../framework/service_manager.h"
#include "../examples/example_services.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace ServiceFramework;

/**
 * @brief Example of a custom service implementation
 *
 * This demonstrates how to create your own service by implementing
 * the IService interface and registering it with the factory.
 */
class WeatherService : public IService {
  public:
    WeatherService() : m_running(false), m_temperature(20.0f) {}

    bool initialize() override {
        std::cout << "WeatherService: Initializing weather monitoring..."
                  << std::endl;
        // Initialize weather sensors, API connections, etc.
        return true;
    }

    bool health() override { return true; }

    bool start() override {
        std::cout << "WeatherService: Starting weather monitoring..."
                  << std::endl;
        m_running = true;

        // Start background monitoring thread (simplified example)
        m_monitoringThread = std::thread([this]() {
            while (m_running) {
                // Simulate temperature reading
                m_temperature =
                    20.0f + (rand() % 20 - 10); // Random temp between 10-30°C
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        });

        return true;
    }

    void stop() override {
        std::cout << "WeatherService: Stopping weather monitoring..."
                  << std::endl;
        m_running = false;

        if (m_monitoringThread.joinable()) {
            m_monitoringThread.join();
        }
    }

    std::string getName() const override { return "WeatherService"; }

    bool isRunning() const override { return m_running; }

    // Custom service methods
    float getCurrentTemperature() const { return m_temperature; }

    std::string getWeatherReport() const {
        if (!m_running) {
            return "Weather service is not running";
        }

        std::string condition;
        if (m_temperature < 10) {
            condition = "Cold";
        } else if (m_temperature < 25) {
            condition = "Mild";
        } else {
            condition = "Warm";
        }

        return "Temperature: " + std::to_string(m_temperature) + "°C (" +
               condition + ")";
    }

  private:
    std::atomic<bool> m_running;
    std::atomic<float> m_temperature;
    std::thread m_monitoringThread;
};

// Register the custom services
namespace {
struct CustomServiceRegistrations {
    CustomServiceRegistrations() {
        auto &factory = ServiceFactory::getInstance();

        factory.registerService("WeatherService", []() -> ServicePtr {
            return std::make_unique<WeatherService>();
        });
    }
};

static CustomServiceRegistrations g_customServiceRegistrations;
} // namespace
