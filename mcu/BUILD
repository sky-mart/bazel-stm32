load("@rules_python//python:defs.bzl", "py_binary")

exports_files(["stm32f303vctx.yaml"])

py_binary(
    name = "geninit",
    data = [
        "board.h.j2",
    ],
    srcs = ["geninit.py"],
    deps = [
        "@pypi//jinja2",
        "@pypi//pyyaml",
    ],
    visibility = ["//visibility:public"],
)
