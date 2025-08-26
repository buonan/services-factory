load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_library(
    name = "weather_service",
    hdrs = ["weather_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//framework:service_interface",
    ],
    strip_include_prefix = ".",
    include_prefix = "services/weather",
)

cc_binary(
    name = "weather_main",
    srcs = ["main.cpp"],
    deps = [
        ":weather_service",
        "//framework:framework_core",
        "//services/logging:logging_service",
        "//services/database:database_service",
        "//services/cache:cache_service",
        "//services/file_monitor:file_monitor_service",
    ],
)
