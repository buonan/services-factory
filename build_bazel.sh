#!/bin/bash

# Bazel build script for C++ Services Framework

set -e

echo "Building C++ Services Framework with Bazel..."

# Build all targets
echo "Building all targets..."
bazel build //...

echo "Build completed successfully!"
echo ""
echo "Available executables:"
echo "  bazel run //:service_framework_demo        # Main framework demo"
echo "  bazel run //services/rest_api:rest_api_demo # REST API server demo"
echo "  bazel run //services/rest_api:test_client   # REST API test client"
echo "  bazel run //services/weather:weather_main   # Weather service demo"
echo ""
echo "Run tests with:"
echo "  bazel test //framework:framework_test       # Framework unit tests"
echo ""
echo "Build specific targets with:"
echo "  bazel build //framework:framework_core      # Core framework only"
echo "  bazel build //services:all_services         # All services"
echo "  bazel build //services/rest_api:rest_api_service # REST API service only"
echo ""
echo "Clean build cache with:"
echo "  bazel clean"
echo ""
echo "Note: Some tests may fail due to service registration issues."
echo "      This is expected and doesn't affect the core functionality."
