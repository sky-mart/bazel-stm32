load("@rules_cc//cc:defs.bzl", "cc_binary")

def _flash_stm32_impl(ctx):
    binary = ctx.file.binary

    script_content = "st-flash write %s 0x08000000" % (binary.short_path)

    script = ctx.actions.declare_file("%s.sh" % ctx.label.name)
    ctx.actions.write(script, script_content, is_executable = True)
    runfiles = ctx.runfiles(files = [binary])
    return [DefaultInfo(executable = script, runfiles = runfiles)]


flash_stm32 = rule(
    implementation = _flash_stm32_impl,
    attrs = {
        "binary": attr.label(mandatory=True, allow_single_file=True),
    },
    executable = True,
)


def _debug_stm32_impl(ctx):
    elf = ctx.file.elf

    script_content = """#!/bin/sh
openocd -f board/stm32f3discovery.cfg &
trap "kill %1" EXIT # kill openocd on exit
arm-none-eabi-gdb -i=mi -q -ex "target remote :3333" -ex "set substitute-path /proc/self/cwd $BUILD_WORKSPACE_DIRECTORY" {elf_to_debug}
""".format(elf_to_debug=elf.short_path)

    script = ctx.actions.declare_file("%s.sh" % ctx.label.name)
    ctx.actions.write(script, script_content, is_executable = True)
    runfiles = ctx.runfiles(files = [elf])
    return [DefaultInfo(executable = script, runfiles = runfiles)]


debug_stm32 = rule(
    implementation = _debug_stm32_impl,
    attrs = {
        "elf": attr.label(mandatory=True, allow_single_file=True),
    },
    executable = True,
)


def stm32f3_binary(**kwargs):
    if "linkopts" not in kwargs:
        kwargs["linkopts"] = []

    kwargs["linkopts"] += [
        "-specs=nano.specs",
        "-specs=nosys.specs",
        "-T $(rootpath //third_party/stm32:STM32F303XC_FLASH.ld)",
    ]

    if "target_compatible_with" not in kwargs:
        kwargs["target_compatible_with"] = []

    kwargs["target_compatible_with"] += [
        "@platforms//cpu:arm",
        "@platforms//os:none",
    ]

    if "deps" not in kwargs:
        kwargs["deps"] = []

    kwargs["deps"] += ["//third_party/stm32:STM32F303XC_FLASH.ld"]

    cc_binary(**kwargs)
