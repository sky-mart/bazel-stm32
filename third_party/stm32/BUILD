load("@arm_none_eabi//toolchain:toolchain.bzl", "arm_none_eabi_toolchain")

arm_none_eabi_toolchain(
    name = "stm32f3",
    copts = [
        "-mcpu=cortex-m4",
        "-fno-builtin",
        "-ffunction-sections",
        "-fdata-sections",
        "-fomit-frame-pointer",
        "-fno-exceptions",
        "-fno-rtti",
    ],
    linkopts = [
        "-mcpu=cortex-m4",
        "-Wl,--gc-sections",
    ],
    target_compatible_with = [
        "@platforms//cpu:arm",
        "@platforms//os:none",
    ],
)

platform(
    name = "stm32f3",
    constraint_values = [
        "@platforms//cpu:arm",
        "@platforms//os:none",
    ],
    visibility = ["//visibility:public"],
)

exports_files(
    [
        "STM32F303XC_FLASH.ld",
    ],
)
