#pragma once
#include <memory>
#include <string>

namespace ServiceFramework {

/**
 * @brief Base interface for all services in the framework
 *
 * All services must inherit from this interface to be managed
 * by the service factory and framework.
 */
class IService {
  public:
    virtual ~IService() = default;

    /**
     * @brief Initialize the service
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize() = 0;

    /**
     * @brief Health
     * @return true if healthy, false otherwise
     */
    virtual bool health() = 0;

    /**
     * @brief Start the service
     * @return true if service started successfully, false otherwise
     */
    virtual bool start() = 0;

    /**
     * @brief Stop the service
     */
    virtual void stop() = 0;

    /**
     * @brief Get the service name
     * @return Service name as string
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Check if service is running
     * @return true if running, false otherwise
     */
    virtual bool isRunning() const = 0;
};

using ServicePtr = std::unique_ptr<IService>;

} // namespace ServiceFramework
