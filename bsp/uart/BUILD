load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "uart",
    srcs = [
        "uart_device.cpp",
        "uart_device.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//bsp/pin",
        "//std_ext:types",
        "@stm32f3xx_hal",
    ],
)
