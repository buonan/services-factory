#include "service_factory.h"
#include <algorithm>
#include <iostream>

namespace ServiceFramework {

ServiceFactory &ServiceFactory::getInstance() {
    static ServiceFactory instance;
    return instance;
}

bool ServiceFactory::registerService(const std::string &serviceName,
                                     ServiceCreator creator) {
    if (serviceName.empty() || !creator) {
        std::cerr << "Invalid service name or creator function" << std::endl;
        return false;
    }

    auto result = m_serviceCreators.emplace(serviceName, std::move(creator));
    if (!result.second) {
        std::cerr << "Service '" << serviceName << "' is already registered"
                  << std::endl;
        return false;
    }

    std::cout << "Service '" << serviceName << "' registered successfully"
              << std::endl;
    return true;
}

ServicePtr ServiceFactory::createService(const std::string &serviceName) {
    auto it = m_serviceCreators.find(serviceName);
    if (it == m_serviceCreators.end()) {
        std::cerr << "Service '" << serviceName << "' not found in registry"
                  << std::endl;
        return nullptr;
    }

    try {
        auto service = it->second();
        if (!service) {
            std::cerr << "Failed to create service '" << serviceName << "'"
                      << std::endl;
            return nullptr;
        }

        std::cout << "Service '" << serviceName << "' created successfully"
                  << std::endl;
        return service;
    } catch (const std::exception &e) {
        std::cerr << "Exception while creating service '" << serviceName
                  << "': " << e.what() << std::endl;
        return nullptr;
    }
}

bool ServiceFactory::isServiceRegistered(const std::string &serviceName) const {
    return m_serviceCreators.find(serviceName) != m_serviceCreators.end();
}

std::vector<std::string> ServiceFactory::getRegisteredServices() const {
    std::vector<std::string> services;
    services.reserve(m_serviceCreators.size());

    for (const auto &pair : m_serviceCreators) {
        services.push_back(pair.first);
    }

    std::sort(services.begin(), services.end());
    return services;
}

bool ServiceFactory::unregisterService(const std::string &serviceName) {
    auto it = m_serviceCreators.find(serviceName);
    if (it == m_serviceCreators.end()) {
        return false;
    }

    m_serviceCreators.erase(it);
    std::cout << "Service '" << serviceName << "' unregistered successfully"
              << std::endl;
    return true;
}

void ServiceFactory::clear() {
    m_serviceCreators.clear();
    std::cout << "All services unregistered" << std::endl;
}

} // namespace ServiceFramework
