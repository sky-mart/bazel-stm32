from jinja2 import Environment, FileSystemLoader
from collections import defaultdict
from dataclasses import dataclass, is_dataclass
from pathlib import Path
import sys
import yaml
import argparse

DEFAULT_GPIO_MODE = "AF_PP"
DEFAULT_GPIO_PULL = "NOPULL"
DEFAULT_GPIO_SPEED = "HIGH"

def with_yaml_loader(cls):
    if not is_dataclass(cls):
        raise TypeError("Decorator can only be applied to dataclasses.")

    @staticmethod
    def from_yaml(path: Path) -> cls:
        with open(path, 'r') as f:
            data = yaml.safe_load(f)
        return cls(**data)

    cls.from_yaml = from_yaml
    return cls

class PeripheralConfig:
    def __init__(self, alias: str, variant: str, type: str, pins: dict[str, int]) -> None:
        self.alias = alias
        self.variant = variant
        self.type = type
        self.pins = pins

@dataclass
class GpioConfig:
    port: str
    pins: set[int]
    mode: str = DEFAULT_GPIO_MODE
    pull: str = DEFAULT_GPIO_PULL
    speed: str = DEFAULT_GPIO_SPEED
    altfun: int = 0
    alias: str | None = None
    pin_array: bool = False

    def __post_init__(self):
        self.pins = set(self.pins)

    def key(self):
        # Used for grouping: ignore `pins`
        return (self.port, self.mode, self.pull, self.speed, self.altfun, self.alias, self.pin_array)

class Project:
    def __init__(self, proj_file: Path) -> None:
        with open(proj_file, 'r') as f:
            root = yaml.safe_load(f)

        self.gpio = []
        for g in root["gpio"]:
            self.gpio.append(GpioConfig(**g))

        self.peripherals = []
        for p in root["peripherals"]:
            self.peripherals.append(PeripheralConfig(**p))


@dataclass
class Gpio:
    bus_interface: str
    ports: list[str]


@dataclass
class GpioPin:
    port: str
    pin: int


class PeripheralVariant:
    def __init__(self, bus_interface: str, alternate_function: int, pins) -> None:
        self.bus_interface = bus_interface
        self.alternate_function = alternate_function
        self.pins = {}
        for name, pin_vars in pins.items():
            self.pins[name] = [GpioPin(**pin_var) for pin_var in pin_vars]


class Peripheral:
    def __init__(self, variants) -> None:
        self.variants = {}
        for name, variant in variants.items():
            self.variants[name] = PeripheralVariant(**variant)


class Mcu:
    def __init__(self, mcu_file: Path) -> None:
        with open(mcu_file, 'r') as f:
            peripherals = yaml.safe_load(f)

        self.peripherals = {}
        for type, peripheral in peripherals.items():
            if type == "mcu_header":
                self.mcu_header = peripheral
            elif type == "GPIO":
                self.peripherals[type] = Gpio(**peripheral)
            else:
                self.peripherals[type] = Peripheral(peripheral)


def squeeze(configs: list[GpioConfig]) -> list[GpioConfig]:
    merged_dict = defaultdict(lambda: None)

    for c in configs:
        k = c.key()
        if merged_dict[k] is None:
            merged_dict[k] = c
    else:
        merged_dict[k].pins.update(c.pins)

    return list(merged_dict.values())


def prefix_items(value, prefix):
    return [f"{prefix}{item}" for item in value]

def postfix_items(value, postfix):
    return [f"{item}{postfix}" for item in value]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-m", "--mcu", type=str, help="The MCU description file")
    parser.add_argument("-p", "--project", type=str, help="The Project description file")
    parser.add_argument("-r", "--output-header", type=str, help="The output header file")
    parser.add_argument("-s", "--output-source", type=str, help="The output source file")
    args = parser.parse_args()

    mcu = Mcu(Path(args.mcu))
    project = Project(Path(args.project))

    gpio_configs = []
    buses = {}
    gpio_bus = mcu.peripherals["GPIO"].bus_interface
    if gpio_bus not in buses:
        buses[gpio_bus] = set()

    for g in project.gpio:
        buses[gpio_bus].add(g.port)

    gpio_configs += project.gpio

    for p in project.peripherals:
        var = p.variant
        per_var = mcu.peripherals[p.type].variants[var]
        bus = per_var.bus_interface
        if bus not in buses:
            buses[bus] = set()
        buses[bus].add(var)

        for pin_name, pin_vars in per_var.pins.items():
            pin = pin_vars[p.pins[pin_name]["variant"]]
            buses[gpio_bus].add(pin.port)

            gpio_configs.append(GpioConfig(port=pin.port, pins=[pin.pin], altfun=per_var.alternate_function))

    gpio_configs = squeeze(gpio_configs)

    env = Environment(loader=FileSystemLoader("mcu"))
    env.filters['prefix_items'] = prefix_items
    env.filters['postfix_items'] = postfix_items

    template = env.get_template("board.h.j2")
    output = template.render(mcu_header = mcu.mcu_header, gpio_configs=gpio_configs, peripherals=project.peripherals)
    Path(args.output_header).write_text(output)

    template = env.get_template("board.cpp.j2")
    output = template.render(buses=buses, gpio_configs=gpio_configs)
    Path(args.output_source).write_text(output)




if __name__ == '__main__':
    main()
