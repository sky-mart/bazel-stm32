load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "ili9341",
    srcs = [
        "ili9341.cpp",
        "ili9341.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//bsp/pin",
        "//bsp/spi",
        "//bsp/utility",
        "@stm32f3xx_hal",
    ],
)
