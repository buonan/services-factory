load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "database_service",
    hdrs = ["database.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//framework:service_interface",
    ],
    strip_include_prefix = ".",
    include_prefix = "services/database",
)
