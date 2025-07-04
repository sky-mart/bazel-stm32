load("@aspect_bazel_lib//lib:transitions.bzl", "platform_transition_filegroup")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//third_party/stm32:defs.bzl", "debug_stm32", "flash_stm32", "stm32f3_binary")

stm32f3_binary(
    name = "imu",
    srcs = [
        "irq_handlers.cpp",
        "main.cpp",
        "syscalls.c",
    ],
    deps = [
        "//bsp/utility",
        "//bsp/stm32f3discovery",
        "//bsp/uart",
        "@cmsis_device_f3",
        "@cmsis//:core",
        "@stm32f3xx_hal",
    ],
)

platform_transition_filegroup(
    name = "elf",
    srcs = [":imu"],
    target_platform = "//third_party/stm32:stm32f3",
)

genrule(
    name = "bin",
    srcs = [":elf"],
    outs = ["imu.bin"],
    cmd = "$(execpath @arm_none_eabi//:objcopy) -O binary $< $@",
    tools = ["@arm_none_eabi//:objcopy"],
    visibility = ["//visibility:public"],
)

flash_stm32(
    name = "flash",
    binary = ":bin",
)

debug_stm32(
    name = "debug",
    elf = ":elf",
)

exports_files(["imu.yaml"])

genrule(
    name = "genboard",
    srcs = [
        ":imu.yaml",
        "//mcu:stm32f303vctx.yaml",
        "//mcu:board.h.j2",
        "//mcu:board.cpp.j2",
    ],
    outs = [
        "board_init.cpp",
        "board_init.h",
    ],
    cmd = "$(location //mcu:geninit) " +
    "--mcu $(location //mcu:stm32f303vctx.yaml) " +
    "--project $(location :imu.yaml) " +
    "--output-header $(location board_init.h) " +
    "--output-source $(location board_init.cpp) ",
    tools = ["//mcu:geninit"],
)

cc_library(
    name = "board_init",
    srcs = [
        "board_init.cpp",
        "board_init.h",
    ],
    deps = [
        ":genboard",
        "@cmsis_device_f3",
        "@cmsis//:core",
        "@stm32f3xx_hal",
    ],
)
