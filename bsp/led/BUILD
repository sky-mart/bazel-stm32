load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "led",
    srcs = [
        "led.cpp",
        "led.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@stm32f3xx_hal",
    ],
)
