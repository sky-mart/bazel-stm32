load("@aspect_bazel_lib//lib:transitions.bzl", "platform_transition_filegroup")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//third_party/stm32:defs.bzl", "debug_stm32", "flash_stm32", "stm32f3_binary")

stm32f3_binary(
    name = "sdcard",
    srcs = [
        "irq_handlers.cpp",
        "main.cpp",
        "wav_header.h",
    ],
    deps = [
        "//bsp/ili9341",
        "//bsp/sdcard",
        "//bsp/stm32f3discovery",
        "//bsp/utility",
        "//fatfs",
        "@stm32f3xx_hal",
    ],
)

platform_transition_filegroup(
    name = "elf",
    srcs = [":sdcard"],
    target_platform = "//third_party/stm32:stm32f3",
)

genrule(
    name = "bin",
    srcs = [":elf"],
    outs = ["sdcard.bin"],
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
