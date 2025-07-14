#include "service_manager.h"
#include <algorithm>
#include <iostream>

namespace ServiceFramework {

ServiceManager::~ServiceManager() { clear(); }

bool ServiceManager::addService(const std::string &serviceName,
                                const std::string &instanceName) {
    auto service = ServiceFactory::getInstance().createService(serviceName);
    if (!service) {
        std::cerr << "Failed to create service: " << serviceName << std::endl;
        return false;
    }

    std::string actualInstanceName =
        instanceName.empty() ? serviceName : instanceName;
    return addService(std::move(service), actualInstanceName);
}

bool ServiceManager::addService(ServicePtr service,
                                const std::string &instanceName) {
    if (!service || instanceName.empty()) {
        std::cerr << "Invalid service or instance name" << std::endl;
        return false;
    }

    if (hasService(instanceName)) {
        std::cerr << "Service instance '" << instanceName << "' already exists"
                  << std::endl;
        return false;
    }

    auto serviceInfo = std::make_unique<ServiceInfo>();
    serviceInfo->service = std::move(service);
    serviceInfo->instanceName = instanceName;

    m_services[instanceName] = std::move(serviceInfo);
    m_serviceOrder.push_back(instanceName);

    std::cout << "Service instance '" << instanceName << "' added successfully"
              << std::endl;
    return true;
}

bool ServiceManager::removeService(const std::string &instanceName) {
    auto it = m_services.find(instanceName);
    if (it == m_services.end()) {
        return false;
    }

    // Stop the service if it's running
    if (it->second->started) {
        it->second->service->stop();
    }

    // Remove from order vector
    auto orderIt =
        std::find(m_serviceOrder.begin(), m_serviceOrder.end(), instanceName);
    if (orderIt != m_serviceOrder.end()) {
        m_serviceOrder.erase(orderIt);
    }

    m_services.erase(it);
    std::cout << "Service instance '" << instanceName
              << "' removed successfully" << std::endl;
    return true;
}

IService *ServiceManager::getService(const std::string &instanceName) const {
    auto it = m_services.find(instanceName);
    return (it != m_services.end()) ? it->second->service.get() : nullptr;
}

bool ServiceManager::initializeAll() {
    std::cout << "Initializing all services..." << std::endl;

    for (const auto &instanceName : m_serviceOrder) {
        auto it = m_services.find(instanceName);
        if (it == m_services.end()) {
            continue;
        }

        auto &serviceInfo = it->second;
        if (serviceInfo->initialized) {
            continue;
        }

        std::cout << "Initializing service: " << instanceName << std::endl;
        if (!serviceInfo->service->initialize()) {
            std::cerr << "Failed to initialize service: " << instanceName
                      << std::endl;
            return false;
        }

        serviceInfo->initialized = true;
        std::cout << "Service '" << instanceName << "' initialized successfully"
                  << std::endl;
    }

    std::cout << "All services initialized successfully" << std::endl;
    return true;
}

bool ServiceManager::startAll() {
    std::cout << "Starting all services..." << std::endl;

    for (const auto &instanceName : m_serviceOrder) {
        auto it = m_services.find(instanceName);
        if (it == m_services.end()) {
            continue;
        }

        auto &serviceInfo = it->second;
        if (!serviceInfo->initialized) {
            std::cerr << "Cannot start uninitialized service: " << instanceName
                      << std::endl;
            return false;
        }

        if (serviceInfo->started) {
            continue;
        }

        std::cout << "Starting service: " << instanceName << std::endl;
        if (!serviceInfo->service->start()) {
            std::cerr << "Failed to start service: " << instanceName
                      << std::endl;
            return false;
        }

        serviceInfo->started = true;
        std::cout << "Service '" << instanceName << "' started successfully"
                  << std::endl;
    }

    std::cout << "All services started successfully" << std::endl;
    return true;
}

void ServiceManager::stopAll() {
    std::cout << "Stopping all services..." << std::endl;

    // Stop services in reverse order
    for (auto it = m_serviceOrder.rbegin(); it != m_serviceOrder.rend(); ++it) {
        const auto &instanceName = *it;
        auto serviceIt = m_services.find(instanceName);
        if (serviceIt == m_services.end()) {
            continue;
        }

        auto &serviceInfo = serviceIt->second;
        if (!serviceInfo->started) {
            continue;
        }

        std::cout << "Stopping service: " << instanceName << std::endl;
        serviceInfo->service->stop();
        serviceInfo->started = false;
        std::cout << "Service '" << instanceName << "' stopped" << std::endl;
    }

    std::cout << "All services stopped" << std::endl;
}

std::vector<std::string> ServiceManager::getServiceNames() const {
    return m_serviceOrder;
}

size_t ServiceManager::getServiceCount() const { return m_services.size(); }

bool ServiceManager::hasService(const std::string &instanceName) const {
    return m_services.find(instanceName) != m_services.end();
}

void ServiceManager::clear() {
    stopAll();
    m_services.clear();
    m_serviceOrder.clear();
    std::cout << "All services cleared" << std::endl;
}

} // namespace ServiceFramework
