#include "rest_api_service.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <fcntl.h>

namespace ServiceFramework {

RestApiService::RestApiService(int port) 
    : m_port(port), m_serverSocket(-1), m_serviceManager(nullptr) {
}

RestApiService::~RestApiService() {
    stop();
}

bool RestApiService::initialize() {
    if (m_initialized.load()) {
        return true;
    }

    try {
        // Create socket
        m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_serverSocket < 0) {
            std::cerr << "RestApiService: Failed to create socket" << std::endl;
            return false;
        }

        // Set socket options
        int opt = 1;
        if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "RestApiService: Failed to set socket options" << std::endl;
            close(m_serverSocket);
            return false;
        }

        // Bind socket
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(m_port);

        if (bind(m_serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "RestApiService: Failed to bind to port " << m_port << std::endl;
            close(m_serverSocket);
            return false;
        }

        // Listen for connections
        if (listen(m_serverSocket, 10) < 0) {
            std::cerr << "RestApiService: Failed to listen on socket" << std::endl;
            close(m_serverSocket);
            return false;
        }

        // Setup default routes
        setupDefaultRoutes();

        // Start worker threads
        for (size_t i = 0; i < MAX_WORKER_THREADS; ++i) {
            m_workerThreads.emplace_back(&RestApiService::workerLoop, this);
        }

        m_initialized.store(true);
        std::cout << "RestApiService: Initialized on port " << m_port << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "RestApiService: Initialization failed: " << e.what() << std::endl;
        return false;
    }
}

bool RestApiService::health() {
    return m_initialized.load() && m_running.load() && m_serverSocket >= 0;
}

bool RestApiService::start() {
    if (!m_initialized.load()) {
        std::cerr << "RestApiService: Cannot start - not initialized" << std::endl;
        return false;
    }

    if (m_running.load()) {
        return true;
    }

    try {
        m_running.store(true);
        m_serverThread = std::thread(&RestApiService::serverLoop, this);
        
        std::cout << "RestApiService: Started HTTP server on port " << m_port << std::endl;
        std::cout << "RestApiService: Available endpoints:" << std::endl;
        std::cout << "  GET    /api/services           - List all services" << std::endl;
        std::cout << "  GET    /api/services/{name}    - Get service info" << std::endl;
        std::cout << "  GET    /api/health/{name}      - Check service health" << std::endl;
        std::cout << "  POST   /api/services/{name}/start - Start service" << std::endl;
        std::cout << "  POST   /api/services/{name}/stop  - Stop service" << std::endl;
        std::cout << "  GET    /api/status             - API server status" << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "RestApiService: Failed to start: " << e.what() << std::endl;
        m_running.store(false);
        return false;
    }
}

void RestApiService::stop() {
    if (!m_running.load()) {
        return;
    }

    std::cout << "RestApiService: Stopping..." << std::endl;
    m_running.store(false);
    m_stopWorkers.store(true);

    // Close server socket to break accept() loop
    if (m_serverSocket >= 0) {
        close(m_serverSocket);
        m_serverSocket = -1;
    }

    // Wait for server thread to finish
    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }

    // Notify all worker threads to stop
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queueCondition.notify_all();
    }

    // Wait for worker threads to finish
    for (auto& worker : m_workerThreads) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    m_workerThreads.clear();

    m_initialized.store(false);
    std::cout << "RestApiService: Stopped" << std::endl;
}

std::string RestApiService::getName() const {
    return "RestApiService";
}

bool RestApiService::isRunning() const {
    return m_running.load();
}

void RestApiService::setServiceManager(ServiceManager* manager) {
    m_serviceManager = manager;
}

void RestApiService::addRoute(const std::string& method, const std::string& path, RouteHandler handler) {
    std::lock_guard<std::mutex> lock(m_routesMutex);
    m_routes[method][path] = handler;
}

void RestApiService::setPort(int port) {
    if (!m_running.load()) {
        m_port = port;
    }
}

int RestApiService::getPort() const {
    return m_port;
}

void RestApiService::serverLoop() {
    while (m_running.load()) {
        struct sockaddr_in clientAddress;
        socklen_t clientLen = sizeof(clientAddress);
        
        int clientSocket = accept(m_serverSocket, (struct sockaddr*)&clientAddress, &clientLen);
        if (clientSocket < 0) {
            if (m_running.load()) {
                std::cerr << "RestApiService: Accept failed" << std::endl;
            }
            continue;
        }

        // Add client to queue for worker threads
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_clientQueue.push_back(clientSocket);
        }
        m_queueCondition.notify_one();
    }
}

void RestApiService::workerLoop() {
    while (!m_stopWorkers.load()) {
        int clientSocket = -1;
        
        // Wait for client connection
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCondition.wait(lock, [this] { 
                return !m_clientQueue.empty() || m_stopWorkers.load(); 
            });
            
            if (m_stopWorkers.load()) {
                break;
            }
            
            if (!m_clientQueue.empty()) {
                clientSocket = m_clientQueue.back();
                m_clientQueue.pop_back();
            }
        }
        
        if (clientSocket >= 0) {
            handleClient(clientSocket);
        }
    }
}

void RestApiService::handleClient(int clientSocket) {
    try {
        // Set socket timeout
        struct timeval timeout;
        timeout.tv_sec = 30;  // 30 seconds timeout
        timeout.tv_usec = 0;
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        // Read request
        char buffer[4096];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesRead <= 0) {
            close(clientSocket);
            return;
        }
        
        buffer[bytesRead] = '\0';
        std::string requestData(buffer);
        
        // Parse and route request
        HttpRequest request = parseRequest(requestData);
        HttpResponse response = routeRequest(request);
        
        // Send response
        std::string responseStr = buildResponse(response);
        send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
        
    } catch (const std::exception& e) {
        std::cerr << "RestApiService: Error handling client: " << e.what() << std::endl;
    }
    
    close(clientSocket);
}

HttpRequest RestApiService::parseRequest(const std::string& requestData) {
    HttpRequest request;
    std::istringstream stream(requestData);
    std::string line;
    
    // Parse request line
    if (std::getline(stream, line)) {
        std::istringstream requestLine(line);
        requestLine >> request.method >> request.path >> request.version;
        
        // Parse query parameters
        size_t queryPos = request.path.find('?');
        if (queryPos != std::string::npos) {
            std::string queryString = request.path.substr(queryPos + 1);
            request.path = request.path.substr(0, queryPos);
            request.queryParams = parseQueryString(queryString);
        }
    }
    
    // Parse headers
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            request.headers[key] = value;
        }
    }
    
    // Parse body (if present)
    std::string body;
    while (std::getline(stream, line)) {
        body += line + "\n";
    }
    if (!body.empty()) {
        body.pop_back(); // Remove last newline
        request.body = body;
    }
    
    return request;
}

std::string RestApiService::buildResponse(const HttpResponse& response) {
    std::ostringstream responseStream;
    
    // Status line
    responseStream << "HTTP/1.1 " << response.statusCode << " " << response.statusText << "\r\n";
    
    // Headers
    for (const auto& header : response.headers) {
        responseStream << header.first << ": " << header.second << "\r\n";
    }
    
    // Content-Length header
    responseStream << "Content-Length: " << response.body.length() << "\r\n";
    responseStream << "Connection: close\r\n";
    responseStream << "\r\n";
    
    // Body
    responseStream << response.body;
    
    return responseStream.str();
}

HttpResponse RestApiService::routeRequest(const HttpRequest& request) {
    std::lock_guard<std::mutex> lock(m_routesMutex);
    
    // Check for exact route match first
    auto methodIt = m_routes.find(request.method);
    if (methodIt != m_routes.end()) {
        auto pathIt = methodIt->second.find(request.path);
        if (pathIt != methodIt->second.end()) {
            return pathIt->second(request);
        }
        
        // Check for parameterized routes
        for (const auto& route : methodIt->second) {
            std::map<std::string, std::string> pathParams;
            if (matchRoute(route.first, request.path, pathParams)) {
                HttpRequest modifiedRequest = request;
                modifiedRequest.pathParams = pathParams;
                return route.second(modifiedRequest);
            }
        }
    }
    
    // Check if path exists for other methods
    for (const auto& methodPair : m_routes) {
        if (methodPair.first != request.method) {
            if (methodPair.second.find(request.path) != methodPair.second.end()) {
                return handleMethodNotAllowed(request);
            }
        }
    }
    
    return handleNotFound(request);
}

void RestApiService::setupDefaultRoutes() {
    // Service management routes
    addRoute("GET", "/api/services", [this](const HttpRequest& req) { return handleServiceList(req); });
    addRoute("GET", "/api/services/{name}", [this](const HttpRequest& req) { return handleServiceInfo(req); });
    addRoute("GET", "/api/health/{name}", [this](const HttpRequest& req) { return handleServiceHealth(req); });
    addRoute("POST", "/api/services/{name}/start", [this](const HttpRequest& req) { return handleServiceStart(req); });
    addRoute("POST", "/api/services/{name}/stop", [this](const HttpRequest& req) { return handleServiceStop(req); });
    
    // API status route
    addRoute("GET", "/api/status", [this](const HttpRequest& req) {
        HttpResponse response;
        response.body = R"({
            "service": "RestApiService",
            "status": "running",
            "port": )" + std::to_string(m_port) + R"(,
            "endpoints": [
                "GET /api/services",
                "GET /api/services/{name}",
                "GET /api/health/{name}",
                "POST /api/services/{name}/start",
                "POST /api/services/{name}/stop",
                "GET /api/status"
            ]
        })";
        return response;
    });
}

HttpResponse RestApiService::handleServiceList(const HttpRequest& request) {
    HttpResponse response;
    
    if (!m_serviceManager) {
        response.statusCode = 503;
        response.statusText = "Service Unavailable";
        response.body = R"({"error": "Service manager not available"})";
        return response;
    }
    
    std::ostringstream json;
    json << R"({"services": [)";
    
    auto services = m_serviceManager->getAllServices();
    bool first = true;
    for (const auto& servicePair : services) {
        if (!first) json << ",";
        json << R"({)";
        json << R"("name": ")" << servicePair.first << R"(",)";
        json << R"("type": ")" << servicePair.second->getName() << R"(",)";
        json << R"("running": )" << (servicePair.second->isRunning() ? "true" : "false");
        json << R"(})";
        first = false;
    }
    
    json << R"(]})";
    response.body = json.str();
    return response;
}

HttpResponse RestApiService::handleServiceInfo(const HttpRequest& request) {
    HttpResponse response;
    
    if (!m_serviceManager) {
        response.statusCode = 503;
        response.statusText = "Service Unavailable";
        response.body = R"({"error": "Service manager not available"})";
        return response;
    }
    
    auto nameIt = request.pathParams.find("name");
    if (nameIt == request.pathParams.end()) {
        response.statusCode = 400;
        response.statusText = "Bad Request";
        response.body = R"({"error": "Service name not provided"})";
        return response;
    }
    
    auto service = m_serviceManager->getService(nameIt->second);
    if (!service) {
        response.statusCode = 404;
        response.statusText = "Not Found";
        response.body = R"({"error": "Service not found"})";
        return response;
    }
    
    std::ostringstream json;
    json << R"({)";
    json << R"("name": ")" << nameIt->second << R"(",)";
    json << R"("type": ")" << service->getName() << R"(",)";
    json << R"("running": )" << (service->isRunning() ? "true" : "false") << R"(,)";
    json << R"("healthy": )" << (service->health() ? "true" : "false");
    json << R"(})";
    
    response.body = json.str();
    return response;
}

HttpResponse RestApiService::handleServiceHealth(const HttpRequest& request) {
    HttpResponse response;
    
    if (!m_serviceManager) {
        response.statusCode = 503;
        response.statusText = "Service Unavailable";
        response.body = R"({"error": "Service manager not available"})";
        return response;
    }
    
    auto nameIt = request.pathParams.find("name");
    if (nameIt == request.pathParams.end()) {
        response.statusCode = 400;
        response.statusText = "Bad Request";
        response.body = R"({"error": "Service name not provided"})";
        return response;
    }
    
    auto service = m_serviceManager->getService(nameIt->second);
    if (!service) {
        response.statusCode = 404;
        response.statusText = "Not Found";
        response.body = R"({"error": "Service not found"})";
        return response;
    }
    
    bool healthy = service->health();
    response.body = R"({"healthy": )" + std::string(healthy ? "true" : "false") + R"(})";
    
    if (!healthy) {
        response.statusCode = 503;
        response.statusText = "Service Unavailable";
    }
    
    return response;
}

HttpResponse RestApiService::handleServiceStart(const HttpRequest& request) {
    HttpResponse response;
    
    if (!m_serviceManager) {
        response.statusCode = 503;
        response.statusText = "Service Unavailable";
        response.body = R"({"error": "Service manager not available"})";
        return response;
    }
    
    auto nameIt = request.pathParams.find("name");
    if (nameIt == request.pathParams.end()) {
        response.statusCode = 400;
        response.statusText = "Bad Request";
        response.body = R"({"error": "Service name not provided"})";
        return response;
    }
    
    auto service = m_serviceManager->getService(nameIt->second);
    if (!service) {
        response.statusCode = 404;
        response.statusText = "Not Found";
        response.body = R"({"error": "Service not found"})";
        return response;
    }
    
    bool started = service->start();
    response.body = R"({"started": )" + std::string(started ? "true" : "false") + R"(})";
    
    if (!started) {
        response.statusCode = 500;
        response.statusText = "Internal Server Error";
    }
    
    return response;
}

HttpResponse RestApiService::handleServiceStop(const HttpRequest& request) {
    HttpResponse response;
    
    if (!m_serviceManager) {
        response.statusCode = 503;
        response.statusText = "Service Unavailable";
        response.body = R"({"error": "Service manager not available"})";
        return response;
    }
    
    auto nameIt = request.pathParams.find("name");
    if (nameIt == request.pathParams.end()) {
        response.statusCode = 400;
        response.statusText = "Bad Request";
        response.body = R"({"error": "Service name not provided"})";
        return response;
    }
    
    auto service = m_serviceManager->getService(nameIt->second);
    if (!service) {
        response.statusCode = 404;
        response.statusText = "Not Found";
        response.body = R"({"error": "Service not found"})";
        return response;
    }
    
    service->stop();
    response.body = R"({"stopped": true})";
    return response;
}

HttpResponse RestApiService::handleNotFound(const HttpRequest& request) {
    HttpResponse response;
    response.statusCode = 404;
    response.statusText = "Not Found";
    response.body = R"({"error": "Endpoint not found"})";
    return response;
}

HttpResponse RestApiService::handleMethodNotAllowed(const HttpRequest& request) {
    HttpResponse response;
    response.statusCode = 405;
    response.statusText = "Method Not Allowed";
    response.body = R"({"error": "Method not allowed for this endpoint"})";
    return response;
}

std::map<std::string, std::string> RestApiService::parseQueryString(const std::string& query) {
    std::map<std::string, std::string> params;
    std::istringstream stream(query);
    std::string pair;
    
    while (std::getline(stream, pair, '&')) {
        size_t equalPos = pair.find('=');
        if (equalPos != std::string::npos) {
            std::string key = urlDecode(pair.substr(0, equalPos));
            std::string value = urlDecode(pair.substr(equalPos + 1));
            params[key] = value;
        }
    }
    
    return params;
}

std::string RestApiService::urlDecode(const std::string& str) {
    std::string decoded;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            std::istringstream hex(str.substr(i + 1, 2));
            if (hex >> std::hex >> value) {
                decoded += static_cast<char>(value);
                i += 2;
            } else {
                decoded += str[i];
            }
        } else if (str[i] == '+') {
            decoded += ' ';
        } else {
            decoded += str[i];
        }
    }
    return decoded;
}

bool RestApiService::matchRoute(const std::string& pattern, const std::string& path, 
                               std::map<std::string, std::string>& pathParams) {
    std::vector<std::string> patternParts;
    std::vector<std::string> pathParts;
    
    // Split pattern and path by '/'
    std::istringstream patternStream(pattern);
    std::istringstream pathStream(path);
    std::string part;
    
    while (std::getline(patternStream, part, '/')) {
        if (!part.empty()) {
            patternParts.push_back(part);
        }
    }
    
    while (std::getline(pathStream, part, '/')) {
        if (!part.empty()) {
            pathParts.push_back(part);
        }
    }
    
    if (patternParts.size() != pathParts.size()) {
        return false;
    }
    
    for (size_t i = 0; i < patternParts.size(); ++i) {
        if (patternParts[i].front() == '{' && patternParts[i].back() == '}') {
            // Parameter placeholder
            std::string paramName = patternParts[i].substr(1, patternParts[i].length() - 2);
            pathParams[paramName] = pathParts[i];
        } else if (patternParts[i] != pathParts[i]) {
            return false;
        }
    }
    
    return true;
}

} // namespace ServiceFramework
