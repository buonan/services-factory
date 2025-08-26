load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "example_services",
    hdrs = ["example_services.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//framework:service_interface",
    ],
    strip_include_prefix = ".",
    include_prefix = "services/examples",
)
