load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "file_monitor_service",
    hdrs = ["file_monitor_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//framework:service_interface",
        "//framework:service_factory",
        "//framework:service_manager",
        "//services/examples:example_services",
    ],
    strip_include_prefix = ".",
    include_prefix = "services/file_monitor",
)
