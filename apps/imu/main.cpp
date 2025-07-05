#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_rcc.h"
#include "stm32f3xx_hal_usart.h"
#include "bsp/uart/uart_device.h"
#include "bsp/stm32f3discovery/board.h"
#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/utility/utility.h"
#include "apps/imu/board_init.h"

using namespace mart;

auto leds = stm32f3discovery::leds();
stm32f3discovery::Gyroscope gyro{};
UartDevice uart{*USART2};

void assert(const bool statement)
{
  if (statement == false)
  {
    __disable_irq();
    while (1)
    {
    }
  }
}

int main()
{
  HAL_Init();

  board::init_clock();
  board::init_gpio();

  gyro.init();
  uart.init(stm32f3discovery::uart_pins(USART2), 115200U);

  while (true)
  {
    const auto whoami = gyro.read(stm32f3discovery::Gyroscope::Register::WHO_AM_I);
    assert(whoami.has_value());
    assert(whoami.value() == 0xD4U);

    const auto current_ms = ms_since_startup();
    assert(uart.printf("%04u: Initialization is successful\r\n", current_ms) == HAL_OK);

    for (auto& led : leds)
    {
      led.toggle();
      delay_ms(250U);
    }
  }
}
