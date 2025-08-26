#include "services/examples/example_services.h"
#include "framework/service_factory.h"
#include "framework/service_manager.h"
#include <cassert>
#include <iostream>

using namespace ServiceFramework;

// Simple test framework
class TestRunner {
  public:
    static void runTest(const std::string &testName,
                        std::function<bool()> testFunc) {
        std::cout << "Running test: " << testName << "... ";
        try {
            if (testFunc()) {
                std::cout << "PASSED" << std::endl;
                s_passedTests++;
            } else {
                std::cout << "FAILED" << std::endl;
                s_failedTests++;
            }
        } catch (const std::exception &e) {
            std::cout << "FAILED (Exception: " << e.what() << ")" << std::endl;
            s_failedTests++;
        }
        s_totalTests++;
    }

    static void printResults() {
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Total tests: " << s_totalTests << std::endl;
        std::cout << "Passed: " << s_passedTests << std::endl;
        std::cout << "Failed: " << s_failedTests << std::endl;
        std::cout << "Success rate: "
                  << (s_totalTests > 0 ? (s_passedTests * 100 / s_totalTests)
                                       : 0)
                  << "%" << std::endl;
    }

    static bool allTestsPassed() { return s_failedTests == 0; }

  private:
    static int s_totalTests;
    static int s_passedTests;
    static int s_failedTests;
};

int TestRunner::s_totalTests = 0;
int TestRunner::s_passedTests = 0;
int TestRunner::s_failedTests = 0;

// Test functions
bool testServiceFactory() {
    auto &factory = ServiceFactory::getInstance();

    // Test service registration check
    if (!factory.isServiceRegistered("LoggingService")) {
        return false;
    }

    // Test service creation
    auto service = factory.createService("LoggingService");
    if (!service) {
        return false;
    }

    // Test service functionality
    if (!service->initialize()) {
        return false;
    }

    if (!service->start()) {
        return false;
    }

    if (!service->isRunning()) {
        return false;
    }

    service->stop();

    if (service->isRunning()) {
        return false;
    }

    return true;
}

bool testServiceManager() {
    ServiceManager manager;

    // Test adding services
    if (!manager.addService("LoggingService", "test_logger")) {
        return false;
    }

    if (!manager.addService("DatabaseService", "test_db")) {
        return false;
    }

    // Test service count
    if (manager.getServiceCount() != 2) {
        return false;
    }

    // Test service existence
    if (!manager.hasService("test_logger")) {
        return false;
    }

    if (!manager.hasService("test_db")) {
        return false;
    }

    // Test getting services
    auto *logger = manager.getService("test_logger");
    if (!logger) {
        return false;
    }

    // Test initialization and starting
    if (!manager.initializeAll()) {
        return false;
    }

    if (!manager.startAll()) {
        return false;
    }

    // Test service is running
    if (!logger->isRunning()) {
        return false;
    }

    // Test stopping
    manager.stopAll();

    if (logger->isRunning()) {
        return false;
    }

    // Test removal
    if (!manager.removeService("test_logger")) {
        return false;
    }

    if (manager.getServiceCount() != 1) {
        return false;
    }

    return true;
}

bool testMultipleServiceInstances() {
    ServiceManager manager;

    // Add multiple instances of the same service type
    if (!manager.addService("CacheService", "cache1")) {
        return false;
    }

    if (!manager.addService("CacheService", "cache2")) {
        return false;
    }

    if (manager.getServiceCount() != 2) {
        return false;
    }

    // Initialize and start
    if (!manager.initializeAll() || !manager.startAll()) {
        return false;
    }

    // Get both instances
    auto *cache1 = dynamic_cast<CacheService *>(manager.getService("cache1"));
    auto *cache2 = dynamic_cast<CacheService *>(manager.getService("cache2"));

    if (!cache1 || !cache2) {
        return false;
    }

    // Test that they are different instances
    if (cache1 == cache2) {
        return false;
    }

    // Test independent operation
    cache1->set("key1", "value1");
    cache2->set("key2", "value2");

    // Each cache should only have its own data
    if (cache1->get("key2") != "" || cache2->get("key1") != "") {
        return false;
    }

    return true;
}

bool testServiceFactoryFeatures() {
    auto &factory = ServiceFactory::getInstance();

    // Test getting registered services
    auto services = factory.getRegisteredServices();
    if (services.empty()) {
        return false;
    }

    // Should have our example services
    bool hasLogging = false, hasDatabase = false, hasNetwork = false,
         hasCache = false;

    for (const auto &service : services) {
        if (service == "LoggingService")
            hasLogging = true;
        if (service == "DatabaseService")
            hasDatabase = true;
        if (service == "NetworkService")
            hasNetwork = true;
        if (service == "CacheService")
            hasCache = true;
    }

    return hasLogging && hasDatabase && hasNetwork && hasCache;
}

bool testErrorHandling() {
    auto &factory = ServiceFactory::getInstance();
    ServiceManager manager;

    // Test creating non-existent service
    auto service = factory.createService("NonExistentService");
    if (service != nullptr) {
        return false;
    }

    // Test adding non-existent service to manager
    if (manager.addService("NonExistentService", "test")) {
        return false;
    }

    // Test getting non-existent service from manager
    auto *nonExistent = manager.getService("non_existent");
    if (nonExistent != nullptr) {
        return false;
    }

    // Test removing non-existent service
    if (manager.removeService("non_existent")) {
        return false;
    }

    return true;
}

int main() {
    std::cout << "Service Framework Unit Tests" << std::endl;
    std::cout << "============================" << std::endl;

    // Run all tests
    TestRunner::runTest("Service Factory Basic", testServiceFactory);
    TestRunner::runTest("Service Manager Basic", testServiceManager);
    TestRunner::runTest("Multiple Service Instances",
                        testMultipleServiceInstances);
    TestRunner::runTest("Service Factory Features", testServiceFactoryFeatures);
    TestRunner::runTest("Error Handling", testErrorHandling);

    // Print results
    TestRunner::printResults();

    return TestRunner::allTestsPassed() ? 0 : 1;
}
