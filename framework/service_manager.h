#pragma once
#include "service_factory.h"
#include "service_interface.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ServiceFramework {

/**
 * @brief Manager class for handling multiple services
 *
 * This class provides lifecycle management for multiple services,
 * including initialization, starting, stopping, and cleanup.
 */
class ServiceManager {
  public:
    ServiceManager() = default;
    ~ServiceManager();

    // Prevent copying
    ServiceManager(const ServiceManager &) = delete;
    ServiceManager &operator=(const ServiceManager &) = delete;

    /**
     * @brief Add a service by creating it from the factory
     * @param serviceName Name of the service type to create
     * @param instanceName Optional instance name (defaults to service name)
     * @return true if service added successfully, false otherwise
     */
    bool addService(const std::string &serviceName,
                    const std::string &instanceName = "");

    /**
     * @brief Add an existing service instance
     * @param service Service instance to add
     * @param instanceName Instance name for the service
     * @return true if service added successfully, false otherwise
     */
    bool addService(ServicePtr service, const std::string &instanceName);

    /**
     * @brief Remove a service by instance name
     * @param instanceName Name of the service instance to remove
     * @return true if service removed, false if not found
     */
    bool removeService(const std::string &instanceName);

    /**
     * @brief Get a service by instance name
     * @param instanceName Name of the service instance
     * @return Pointer to the service, nullptr if not found
     */
    IService *getService(const std::string &instanceName) const;

    /**
     * @brief Initialize all services
     * @return true if all services initialized successfully, false otherwise
     */
    bool initializeAll();

    /**
     * @brief Start all services
     * @return true if all services started successfully, false otherwise
     */
    bool startAll();

    /**
     * @brief Stop all services
     */
    void stopAll();

    /**
     * @brief Get list of all service instance names
     * @return Vector of service instance names
     */
    std::vector<std::string> getServiceNames() const;

    /**
     * @brief Get count of managed services
     * @return Number of services
     */
    size_t getServiceCount() const;

    /**
     * @brief Check if a service instance exists
     * @param instanceName Name of the service instance
     * @return true if exists, false otherwise
     */
    bool hasService(const std::string &instanceName) const;

    /**
     * @brief Clear all services (stops and removes them)
     */
    void clear();

    /**
     * @brief Get all services as a map of instance name to service pointer
     * @return Map of instance names to service pointers
     */
    std::unordered_map<std::string, IService*> getAllServices() const;

  private:
    struct ServiceInfo {
        ServicePtr service;
        std::string instanceName;
        bool initialized = false;
        bool started = false;
    };

    std::unordered_map<std::string, std::unique_ptr<ServiceInfo>> m_services;
    std::vector<std::string> m_serviceOrder; // Maintains insertion order
};

} // namespace ServiceFramework
