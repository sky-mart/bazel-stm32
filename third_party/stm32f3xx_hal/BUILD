cc_library(
    name = "stm32f3xx_hal",
    hdrs = glob(["Inc/**/*.h"]),
    strip_include_prefix = "Inc",
    srcs = glob(["Src/**/*.c"]),
    visibility = ["//visibility:public"],
    deps = [
        "@cmsis//:core",
        "@cmsis_device_f3",
        "@stm32f3xx_hal_conf",
    ],
)
