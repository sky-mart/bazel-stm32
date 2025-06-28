from jinja2 import Environment, FileSystemLoader
from dataclasses import dataclass, is_dataclass
from pathlib import Path
import sys
import yaml

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

class GpioConfig:
    def __init__(self, port: str, mode: str, pull: str, speed: str, pins: set[str], altfun = 0) -> None:
        self.port = port
        self.mode = mode
        self.pull = pull
        self.speed = speed
        self.altfun = altfun
        self.pins = set(pins)

    def __repr__(self):
        return f"GpioConfig(port: {self.port}, mode: {self.mode}, pull: {self.pull}, speed: {self.speed}, pins: {list(self.pins)}"

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
            if type == "GPIO":
                self.peripherals[type] = Gpio(**peripheral)
            else:
                self.peripherals[type] = Peripheral(peripheral)


def squeeze(configs: list[GpioConfig]) -> list[GpioConfig]:
    sorted = {}
    for c in configs:
        # print(c)
        if c.port not in sorted:
            sorted[c.port] = {}

        if c.mode not in sorted[c.port]:
            sorted[c.port][c.mode] = {}

        if c.pull not in sorted[c.port][c.mode]:
            sorted[c.port][c.mode][c.pull] = {}

        if c.speed not in sorted[c.port][c.mode][c.pull]:
            sorted[c.port][c.mode][c.pull][c.speed] = set()

        sorted[c.port][c.mode][c.pull][c.speed].update(c.pins)

    result = []
    for port, configs_by_mode in sorted.items():
        for mode, configs_by_pull in configs_by_mode.items():
            for pull, configs_by_speed in configs_by_pull.items():
                for speed, pins in configs_by_speed.items():
                    result.append(GpioConfig(port, mode, pull, speed, pins))
    return result


def prefix_items(value, prefix):
    return [f"{prefix}{item}" for item in value]

def main():
    project_file = sys.argv[1]
    mcu_file = sys.argv[2]

    mcu = Mcu(Path(mcu_file))
    project = Project(Path(project_file))

    gpio_configs = []
    buses = {}
    gpio_bus = mcu.peripherals["GPIO"].bus_interface
    if gpio_bus not in buses:
        buses[gpio_bus] = set()

    for g in project.gpio:
        buses[gpio_bus].add(f"RCC_{gpio_bus}ENR_{g.port}EN")

    gpio_configs += project.gpio

    for p in project.peripherals:
        var = p.variant
        per_var = mcu.peripherals[p.type].variants[var]
        bus = per_var.bus_interface
        if bus not in buses:
            buses[bus] = set()
        buses[bus].add(f"RCC_{bus}ENR_{var}EN")

        for pin_name, pin_vars in per_var.pins.items():
            pin = pin_vars[p.pins[pin_name]]
            buses[gpio_bus].add(f"RCC_{gpio_bus}ENR_{pin.port}EN")

            gpio_configs.append(GpioConfig(port=pin.port, pins=[pin.pin], mode="ALT", pull="UNK", speed="UNK", altfun=per_var.alternate_function))

    # print(buses)
    # print(squeeze(gpio_configs))

    env = Environment(loader=FileSystemLoader("mcu"))
    env.filters['prefix_items'] = prefix_items
    # template = env.get_template("board.h.j2")
    # output = template.render(mcu_header = "stm32f303xc.h", peripherals=project.peripherals)
    # print(output)

    print(env.list_templates())
    template = env.get_template("board.cpp.j2")
    output = template.render(buses=buses, gpio_configs=squeeze(gpio_configs))
    print(output)




if __name__ == '__main__':
    main()
