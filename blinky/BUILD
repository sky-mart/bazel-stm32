load("@aspect_bazel_lib//lib:transitions.bzl", "platform_transition_filegroup")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//third_party/stm32:defs.bzl", "debug_stm32", "flash_stm32", "stm32f3_binary")

cc_library(
    name = "startup",
    srcs = [
        "startup_stm32f303xc.S",
        "stm32f3xx.h",
        "stm32f303xc.h",
        "system_stm32f3xx.c",
        "system_stm32f3xx.h",
    ],
    target_compatible_with = [
        "@platforms//cpu:arm",
        "@platforms//os:none",
    ],
    deps = [
        "@CMSIS//:Core",
    ],
)

# cc_binary(
#     name = "blinky",
#     srcs = [
#         "main.cpp",
#     ],
#     linkopts = [
#         "-specs=nano.specs",
#         "-specs=nosys.specs",
#         "-T $(rootpath STM32F303XC_FLASH.ld)",
#     ],
#     target_compatible_with = [
#         "@platforms//cpu:arm",
#         "@platforms//os:none",
#     ],
#     deps = [
#         "STM32F303XC_FLASH.ld",
#         ":startup",
#         "@CMSIS//:Core",
#     ],
# )

stm32f3_binary(
    name = "blinky",
    srcs = [
        "main.cpp",
    ],
    deps = [
        ":startup",
         "@CMSIS//:Core",
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
