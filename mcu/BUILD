load("@rules_python//python:defs.bzl", "py_binary")

exports_files(["stm32f303vctx.yaml"])

py_binary(
    name = "geninit",
    srcs = ["geninit.py"],
)
