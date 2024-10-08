cc_library(
    name = "cmsis_device_f3",
    hdrs = glob(["Include/**/*.h"]),
    strip_include_prefix = "Include",
    srcs = [
         "Source/Templates/system_stm32f3xx.c",
         "Source/Templates/gcc/startup_stm32f303xc.S",
    ],
    visibility = ["//visibility:public"],
    deps = ["@cmsis//:core"],
)
