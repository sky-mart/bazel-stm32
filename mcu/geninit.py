from jinja2 import Environment, FileSystemLoader
from dataclasses import dataclass

@dataclass
class Peripheral:
    alias: str
    variant: str


def main():
    env = Environment(loader=FileSystemLoader("mcu"))
    template = env.get_template("board.h.j2")
    output = template.render(mcu_header = "stm32f303xc.h", peripherals=[
        Peripheral("CONSOLE_USART", "USART2"),
        Peripheral("GYRO_SPI", "SPI1"),
    ])
    print(output)

if __name__ == '__main__':
    main()
