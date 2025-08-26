load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary", "cc_test")

# Core service interface
cc_library(
    name = "service_interface",
    hdrs = ["service_interface.h"],
    visibility = ["//visibility:public"],
    strip_include_prefix = ".",
    include_prefix = "framework",
)

# Service factory
cc_library(
    name = "service_factory",
    srcs = ["service_factory.cpp"],
    hdrs = ["service_factory.h"],
    visibility = ["//visibility:public"],
    deps = [":service_interface"],
    strip_include_prefix = ".",
    include_prefix = "framework",
)

# Service manager
cc_library(
    name = "service_manager",
    srcs = ["service_manager.cpp"],
    hdrs = ["service_manager.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":service_interface",
        ":service_factory",
    ],
    strip_include_prefix = ".",
    include_prefix = "framework",
)

# Framework core (combines all framework components)
cc_library(
    name = "framework_core",
    visibility = ["//visibility:public"],
    deps = [
        ":service_interface",
        ":service_factory",
        ":service_manager",
    ],
)

# Framework tests
cc_test(
    name = "framework_test",
    srcs = ["test/test_framework.cpp"],
    deps = [
        ":framework_core",
        "//services/logging:logging_service",
        "//services/database:database_service",
        "//services/cache:cache_service",
        "//services/examples:example_services",
        "@googletest//:gtest_main",
    ],
)
