load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "logging_service",
    hdrs = ["logging_service.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//framework:service_interface",
    ],
    strip_include_prefix = ".",
    include_prefix = "services/logging",
)
