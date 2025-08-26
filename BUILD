load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

# Main service framework library (aggregates framework components)
cc_library(
    name = "service_framework",
    visibility = ["//visibility:public"],
    deps = [
        "//framework:framework_core",
        "//services/rest_api:rest_api_service",
    ],
)

# All services library (aggregates all service implementations)
cc_library(
    name = "all_services",
    visibility = ["//visibility:public"],
    deps = [
        "//services:all_services",
    ],
)

# Main demo executable
cc_binary(
    name = "service_framework_demo",
    srcs = ["main.cpp"],
    deps = [
        ":service_framework",
        ":all_services",
    ],
)
