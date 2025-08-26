load("@rules_cc//cc:defs.bzl", "cc_library")

# Aggregate library for all services
cc_library(
    name = "all_services",
    visibility = ["//visibility:public"],
    deps = [
        "//services/cache:cache_service",
        "//services/database:database_service",
        "//services/examples:example_services",
        "//services/file_monitor:file_monitor_service",
        "//services/logging:logging_service",
        "//services/network:network_service",
        "//services/rest_api:rest_api_service",
        "//services/weather:weather_service",
    ],
)
