#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class SimpleHttpClient {
public:
    static std::string makeRequest(const std::string& method, const std::string& path, 
                                 const std::string& body = "", int port = 8080) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            return "Error: Could not create socket";
        }
        
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            close(sock);
            return "Error: Could not connect to server";
        }
        
        // Build HTTP request
        std::string request = method + " " + path + " HTTP/1.1\r\n";
        request += "Host: localhost:" + std::to_string(port) + "\r\n";
        request += "Connection: close\r\n";
        
        if (!body.empty()) {
            request += "Content-Length: " + std::to_string(body.length()) + "\r\n";
            request += "Content-Type: application/json\r\n";
        }
        
        request += "\r\n";
        if (!body.empty()) {
            request += body;
        }
        
        // Send request
        if (send(sock, request.c_str(), request.length(), 0) < 0) {
            close(sock);
            return "Error: Could not send request";
        }
        
        // Read response
        std::string response;
        char buffer[4096];
        ssize_t bytesRead;
        
        while ((bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }
        
        close(sock);
        return response;
    }
    
    static std::string extractBody(const std::string& response) {
        size_t bodyStart = response.find("\r\n\r\n");
        if (bodyStart != std::string::npos) {
            return response.substr(bodyStart + 4);
        }
        return response;
    }
};

void testEndpoint(const std::string& method, const std::string& path, 
                 const std::string& body = "") {
    std::cout << "\n--- Testing " << method << " " << path << " ---" << std::endl;
    
    std::string response = SimpleHttpClient::makeRequest(method, path, body);
    if (response.find("Error:") == 0) {
        std::cout << "Request failed: " << response << std::endl;
        return;
    }
    
    // Extract and display just the body
    std::string responseBody = SimpleHttpClient::extractBody(response);
    std::cout << "Response: " << responseBody << std::endl;
}

int main() {
    std::cout << "=== REST API Test Client ===" << std::endl;
    std::cout << "Make sure the REST API demo is running on port 8080" << std::endl;
    
    // Test various endpoints
    testEndpoint("GET", "/api/status");
    testEndpoint("GET", "/api/services");
    testEndpoint("GET", "/api/services/logger");
    testEndpoint("GET", "/api/services/primary_db");
    testEndpoint("GET", "/api/health/logger");
    testEndpoint("GET", "/api/custom/hello");
    testEndpoint("POST", "/api/custom/echo", R"({"test": "Hello from test client!"})");
    
    // Test service control
    testEndpoint("POST", "/api/services/logger/stop");
    testEndpoint("GET", "/api/services/logger");
    testEndpoint("POST", "/api/services/logger/start");
    testEndpoint("GET", "/api/services/logger");
    
    // Test error cases
    testEndpoint("GET", "/api/services/nonexistent");
    testEndpoint("GET", "/api/nonexistent/endpoint");
    testEndpoint("POST", "/api/services"); // Method not allowed
    
    std::cout << "\n=== Test completed ===" << std::endl;
    return 0;
}
