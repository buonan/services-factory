cmake_minimum_required(VERSION 3.12)
project(ServiceFramework VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add debug information in debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Create the service framework library
add_library(ServiceFramework STATIC
    ./framework/service_factory.cpp
    ./framework/service_manager.cpp
)

target_include_directories(ServiceFramework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Create the main executable
add_executable(ServiceFrameworkDemo
    ./services/weather/main.cpp
    # main.cpp
)

target_link_libraries(ServiceFrameworkDemo
    ServiceFramework
    pthread  # For std::thread support
)

# Optional: Create a simple test executable
add_executable(ServiceFrameworkTest
    ./framework/test/test_framework.cpp
)

target_link_libraries(ServiceFrameworkTest
    ServiceFramework
    pthread
)

# Installation rules (optional)
install(TARGETS ServiceFramework ServiceFrameworkDemo
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    service_interface.h
    service_factory.h
    service_manager.h
    example_services.h
    DESTINATION include/ServiceFramework
)
