load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "stm32f3discovery",
    srcs = [
        "board.cpp",
        "board.h",
        "gyroscope.cpp",
        "gyroscope.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//bsp/pin",
        "//bsp/spi",
    ],
)
