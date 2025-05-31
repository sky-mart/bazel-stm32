load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "spi",
    srcs = [
        "chip_select.h",
        "spi_device.cpp",
        "spi_device.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//bsp/pin",
        "//std_ext:types",
        "@stm32f3xx_hal",
    ],
)
