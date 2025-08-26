#pragma once

#include "framework/service_interface.h"
#include "framework/service_manager.h"
#include <atomic>
#include <thread>
#include <string>
#include <map>
#include <functional>
#include <vector>
#include <sstream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace ServiceFramework {

/**
 * @brief HTTP Request structure
 */
struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> queryParams;
    std::map<std::string, std::string> pathParams;
};

/**
 * @brief HTTP Response structure
 */
struct HttpResponse {
    int statusCode = 200;
    std::string statusText = "OK";
    std::map<std::string, std::string> headers;
    std::string body;
    
    HttpResponse() {
        headers["Content-Type"] = "application/json";
        headers["Server"] = "ServiceFramework-REST/1.0";
    }
};

/**
 * @brief Route handler function type
 */
using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;

/**
 * @brief RESTful API Service
 * 
 * Provides HTTP REST API endpoints to interact with other services
 * in the framework. Supports GET, POST, PUT, DELETE methods and
 * can route requests to appropriate service methods.
 */
class RestApiService : public IService {
public:
    RestApiService(int port = 8080);
    virtual ~RestApiService();

    // IService interface implementation
    bool initialize() override;
    bool health() override;
    bool start() override;
    void stop() override;
    std::string getName() const override;
    bool isRunning() const override;

    // REST API specific methods
    void setServiceManager(ServiceManager* manager);
    void addRoute(const std::string& method, const std::string& path, RouteHandler handler);
    void setPort(int port);
    int getPort() const;

private:
    // HTTP server methods
    void serverLoop();
    void handleClient(int clientSocket);
    HttpRequest parseRequest(const std::string& requestData);
    std::string buildResponse(const HttpResponse& response);
    HttpResponse routeRequest(const HttpRequest& request);
    
    // Built-in route handlers
    HttpResponse handleServiceList(const HttpRequest& request);
    HttpResponse handleServiceStatus(const HttpRequest& request);
    HttpResponse handleServiceHealth(const HttpRequest& request);
    HttpResponse handleServiceStart(const HttpRequest& request);
    HttpResponse handleServiceStop(const HttpRequest& request);
    HttpResponse handleServiceInfo(const HttpRequest& request);
    HttpResponse handleNotFound(const HttpRequest& request);
    HttpResponse handleMethodNotAllowed(const HttpRequest& request);
    
    // Utility methods
    std::map<std::string, std::string> parseQueryString(const std::string& query);
    std::string urlDecode(const std::string& str);
    bool matchRoute(const std::string& pattern, const std::string& path, 
                   std::map<std::string, std::string>& pathParams);

    // Member variables
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_initialized{false};
    int m_port;
    int m_serverSocket;
    std::thread m_serverThread;
    ServiceManager* m_serviceManager;
    
    // Route management
    std::map<std::string, std::map<std::string, RouteHandler>> m_routes; // method -> path -> handler
    std::mutex m_routesMutex;
    
    // Thread pool for handling requests
    std::vector<std::thread> m_workerThreads;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    std::vector<int> m_clientQueue;
    std::atomic<bool> m_stopWorkers{false};
    static const size_t MAX_WORKER_THREADS = 10;
    
    void workerLoop();
    void setupDefaultRoutes();
};

} // namespace ServiceFramework
