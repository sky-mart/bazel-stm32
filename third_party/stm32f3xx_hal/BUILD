cc_library(
    name = "stm32f3xx_hal",
    hdrs = [
        "Inc/stm32f3xx_hal.h",
        "Inc/stm32f3xx_hal_cortex.h",
        "Inc/stm32f3xx_hal_def.h",
        "Inc/stm32f3xx_hal_flash.h",
        "Inc/stm32f3xx_hal_flash_ex.h",
        "Inc/stm32f3xx_hal_gpio.h",
        "Inc/stm32f3xx_hal_gpio_ex.h",
        "Inc/stm32f3xx_hal_rcc.h",
        "Inc/stm32f3xx_hal_rcc_ex.h",
        "Inc/Legacy/stm32_hal_legacy.h",
    ],
    strip_include_prefix = "Inc",
    includes = ["Inc"],
    srcs = [
        "Src/stm32f3xx_hal.c",
        "Src/stm32f3xx_hal_cortex.c",
        "Src/stm32f3xx_hal_gpio.c",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@cmsis//:core",
        "@cmsis_device_f3",
        "@stm32f3xx_hal_conf",
    ],
)
