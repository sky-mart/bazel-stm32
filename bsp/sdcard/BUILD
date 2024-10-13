load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "sdcard",
    srcs = [
        "sdcard.cpp",
        "sdcard.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//bsp/spi",
        "@stm32f3xx_hal",
    ],
)
