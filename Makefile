# C++ Services Framework Makefile

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g
LDFLAGS = -pthread

# Directories
BUILD_DIR = build
SRC_DIR = .
FRAMEWORK_DIR = framework
SERVICES_DIR = services

# Source files
FRAMEWORK_SOURCES = $(FRAMEWORK_DIR)/service_factory.cpp $(FRAMEWORK_DIR)/service_manager.cpp
REST_API_SOURCES = $(SERVICES_DIR)/rest_api/rest_api_service.cpp
ALL_SOURCES = $(FRAMEWORK_SOURCES) $(REST_API_SOURCES)

# Object files
FRAMEWORK_OBJECTS = $(FRAMEWORK_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
REST_API_OBJECTS = $(REST_API_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
ALL_OBJECTS = $(ALL_SOURCES:%.cpp=$(BUILD_DIR)/%.o)

# Targets
LIBRARY = $(BUILD_DIR)/libServiceFramework.a
DEMO_TARGET = $(BUILD_DIR)/ServiceFrameworkDemo
REST_API_DEMO = $(BUILD_DIR)/RestApiDemo
REST_API_CLIENT = $(BUILD_DIR)/RestApiTestClient
TEST_TARGET = $(BUILD_DIR)/ServiceFrameworkTest

# Default target
all: $(LIBRARY) $(DEMO_TARGET) $(REST_API_DEMO) $(REST_API_CLIENT) $(TEST_TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(FRAMEWORK_DIR)
	mkdir -p $(BUILD_DIR)/$(SERVICES_DIR)/rest_api
	mkdir -p $(BUILD_DIR)/$(SERVICES_DIR)/weather

# Build static library
$(LIBRARY): $(ALL_OBJECTS) | $(BUILD_DIR)
	ar rcs $@ $^
	@echo "Library built: $@"

# Build object files
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# Build main demo
$(DEMO_TARGET): $(SERVICES_DIR)/weather/main.cpp $(LIBRARY) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -L$(BUILD_DIR) -lServiceFramework $(LDFLAGS) -o $@

# Build REST API demo
$(REST_API_DEMO): $(SERVICES_DIR)/rest_api/rest_api_demo.cpp $(LIBRARY) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -L$(BUILD_DIR) -lServiceFramework $(LDFLAGS) -o $@

# Build REST API test client
$(REST_API_CLIENT): $(SERVICES_DIR)/rest_api/test_client.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# Build test executable
$(TEST_TARGET): $(FRAMEWORK_DIR)/test/test_framework.cpp $(LIBRARY) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -L$(BUILD_DIR) -lServiceFramework $(LDFLAGS) -o $@

# Run targets
run-demo: $(DEMO_TARGET)
	@echo "Running main demo..."
	./$(DEMO_TARGET)

run-rest-api: $(REST_API_DEMO)
	@echo "Starting REST API server..."
	@echo "Server will run on http://localhost:8080"
	@echo "Press Ctrl+C to stop"
	./$(REST_API_DEMO)

run-test-client: $(REST_API_CLIENT)
	@echo "Running REST API test client..."
	@echo "Make sure REST API server is running first!"
	./$(REST_API_CLIENT)

run-test: $(TEST_TARGET)
	@echo "Running framework tests..."
	./$(TEST_TARGET)

# CMake build (alternative)
cmake-build:
	mkdir -p build
	cd build && cmake .. && make

cmake-run-rest-api: cmake-build
	@echo "Starting REST API server (CMake build)..."
	./build/RestApiDemo

# Clean targets
clean:
	rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned"

clean-cmake:
	rm -rf build
	@echo "CMake build directory cleaned"

# Help
help:
	@echo "Available targets:"
	@echo "  all              - Build everything"
	@echo "  $(LIBRARY)       - Build static library"
	@echo "  $(DEMO_TARGET)   - Build main demo"
	@echo "  $(REST_API_DEMO) - Build REST API demo"
	@echo "  $(REST_API_CLIENT) - Build REST API test client"
	@echo "  $(TEST_TARGET)   - Build test executable"
	@echo ""
	@echo "Run targets:"
	@echo "  run-demo         - Run main demo"
	@echo "  run-rest-api     - Run REST API server"
	@echo "  run-test-client  - Run REST API test client"
	@echo "  run-test         - Run framework tests"
	@echo ""
	@echo "CMake targets:"
	@echo "  cmake-build      - Build using CMake"
	@echo "  cmake-run-rest-api - Run REST API using CMake build"
	@echo ""
	@echo "Clean targets:"
	@echo "  clean            - Clean Makefile build"
	@echo "  clean-cmake      - Clean CMake build"
	@echo "  help             - Show this help"

.PHONY: all run-demo run-rest-api run-test-client run-test cmake-build cmake-run-rest-api clean clean-cmake help
