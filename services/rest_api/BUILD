load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_library(
    name = "rest_api_service",
    srcs = ["rest_api_service.cpp"],
    hdrs = [
        "rest_api_service.h",
        "rest_api_registration.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//framework:service_interface",
        "//framework:service_manager",
    ],
)

cc_binary(
    name = "rest_api_demo",
    srcs = ["rest_api_demo.cpp"],
    deps = [
        ":rest_api_service",
        "//framework:framework_core",
        "//services/logging:logging_service",
        "//services/database:database_service",
        "//services/cache:cache_service",
    ],
)

cc_binary(
    name = "test_client",
    srcs = ["test_client.cpp"],
    deps = [
        "//framework:framework_core",
    ],
)
