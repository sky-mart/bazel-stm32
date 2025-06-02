load("@rules_uv//uv:pip.bzl", "pip_compile")

pip_compile(
    name = "python_requirements",
    requirements_in = "requirements.in",
    requirements_txt = "requirements_lock.txt",
)
