#pragma once
#include "service_interface.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ServiceFramework {

/**
 * @brief Factory class for creating services using the factory pattern
 *
 * This class manages service creation through registered factory functions.
 * Services can be registered at runtime and created by name.
 */
class ServiceFactory {
  public:
    using ServiceCreator = std::function<ServicePtr()>;

    /**
     * @brief Get the singleton instance of the factory
     * @return Reference to the factory instance
     */
    static ServiceFactory &getInstance();

    /**
     * @brief Register a service creator function
     * @param serviceName Name of the service type
     * @param creator Function that creates the service instance
     * @return true if registration successful, false if service already exists
     */
    bool registerService(const std::string &serviceName,
                         ServiceCreator creator);

    /**
     * @brief Create a service instance by name
     * @param serviceName Name of the service type to create
     * @return Unique pointer to the created service, nullptr if not found
     */
    ServicePtr createService(const std::string &serviceName);

    /**
     * @brief Check if a service type is registered
     * @param serviceName Name of the service type
     * @return true if registered, false otherwise
     */
    bool isServiceRegistered(const std::string &serviceName) const;

    /**
     * @brief Get list of all registered service names
     * @return Vector of registered service names
     */
    std::vector<std::string> getRegisteredServices() const;

    /**
     * @brief Unregister a service type
     * @param serviceName Name of the service type to unregister
     * @return true if unregistered, false if not found
     */
    bool unregisterService(const std::string &serviceName);

    /**
     * @brief Clear all registered services
     */
    void clear();

  private:
    ServiceFactory() = default;
    ~ServiceFactory() = default;

    // Prevent copying
    ServiceFactory(const ServiceFactory &) = delete;
    ServiceFactory &operator=(const ServiceFactory &) = delete;

    std::unordered_map<std::string, ServiceCreator> m_serviceCreators;
};

// Macro to simplify service registration
#define REGISTER_SERVICE(ServiceType, ServiceName)                             \
    namespace {                                                                \
    struct ServiceRegistrar_##ServiceType {                                    \
        ServiceRegistrar_##ServiceType() {                                     \
            ServiceFramework::ServiceFactory::getInstance().registerService(   \
                ServiceName, []() -> ServiceFramework::ServicePtr {            \
                    return std::make_unique<ServiceType>();                    \
                });                                                            \
        }                                                                      \
    };                                                                         \
    static ServiceRegistrar_##ServiceType g_registrar_##ServiceType;           \
    }

} // namespace ServiceFramework
