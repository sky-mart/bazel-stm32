from jinja2 import Environment, FileSystemLoader
from dataclasses import dataclass
from pathlib import Path
import sys
import yaml

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


def project_configs(project_file: Path) -> (list[PeripheralConfig], list[GpioConfig]):
    with project_file.open("r") as f:
        project = yaml.safe_load(f)

    return [PeripheralConfig(**peripheral) for peripheral in project["peripherals"]], [GpioConfig(**gpio) for gpio in project["gpio"]]


def mcu_descr(mcu_file: Path):
    with mcu_file.open("r") as f:
        mcu = yaml.safe_load(f)

    for peripheral_type in mcu:
        if peripheral_type == "GPIO":
            Gpio(**mcu[peripheral_type])
        else:
            for peripheral_variant in mcu[peripheral_type]:
                PeripheralVariant(**mcu[peripheral_type][peripheral_variant])

def main():
    project_file = sys.argv[1]
    mcu_file = sys.argv[2]

    peripherals, gpio = project_configs(Path(project_file))
    mcu_descr(Path(mcu_file))

    env = Environment(loader=FileSystemLoader("mcu"))
    template = env.get_template("board.h.j2")
    output = template.render(mcu_header = "stm32f303xc.h", peripherals=peripherals)
    print(output)


if __name__ == '__main__':
    main()
