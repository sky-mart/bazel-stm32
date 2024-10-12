load("@aspect_bazel_lib//lib:transitions.bzl", "platform_transition_filegroup")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//third_party/stm32:defs.bzl", "debug_stm32", "flash_stm32", "stm32f3_binary")

stm32f3_binary(
    name = "blinky",
    srcs = [
        "irq_handlers.cpp",
        "main.cpp",
    ],
    deps = [
        "//bsp/led",
        "//bsp/utility",
        "//bsp/stm32f3discovery",
        "@cmsis_device_f3",
        "@cmsis//:core",
        "@stm32f3xx_hal",
    ],
)

platform_transition_filegroup(
    name = "elf",
    srcs = [":blinky"],
    target_platform = "//third_party/stm32:stm32f3",
)

genrule(
    name = "bin",
    srcs = [":elf"],
    outs = ["blinky.bin"],
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
