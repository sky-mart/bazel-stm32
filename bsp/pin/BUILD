load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "pin",
    srcs = [
        "pin.cpp",
        "pin.h",
        "pin_array.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@stm32f3xx_hal",
    ],
)
