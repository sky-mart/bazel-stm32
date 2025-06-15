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

@dataclass
class PeripheralConfig:
    alias: str
    variant: str
    type: str
    pins: dict[str, int]

@dataclass
class GpioConfig:
    port: str
    pins: list[int]
    mode: str
    pull: str
    speed: str

@with_yaml_loader
@dataclass
class Project:
    gpio: list[GpioConfig]
    peripherals: list[PeripheralConfig]


@dataclass
class Gpio:
    bus_interface: str
    ports: list[str]


@dataclass
class GpioPin:
    port: str
    pin: int


@dataclass
class PeripheralVariant:
    bus_interface: str
    alternate_function: int
    pins: dict[str, list[GpioPin]]


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


def main():
    project_file = sys.argv[1]
    mcu_file = sys.argv[2]

    # project = Project.from_yaml(Path(project_file))
    mcu = Mcu(Path(mcu_file))
    print(mcu.peripherals)

    env = Environment(loader=FileSystemLoader("mcu"))
    template = env.get_template("board.h.j2")
    # output = template.render(mcu_header = "stm32f303xc.h", peripherals=project.peripherals)
    # print(output)


if __name__ == '__main__':
    main()
