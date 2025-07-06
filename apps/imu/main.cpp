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
UartDevice uart{*CONSOLE_USART};

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

  auto result = gyro.init();
  assert(result == HAL_OK && "Gyro init failed");

  result = uart.init(115200U);
  assert(result == HAL_OK && "UART init failed");

  while (true)
  {
    // const auto whoami = gyro.read(stm32f3discovery::Gyroscope::Register::WHO_AM_I);
    // assert(whoami.has_value());
    // assert(whoami.value() == 0xD4U);

    const auto current_ms = ms_since_startup();
    assert(uart.printf("%04u: Initialization is successful\r\n", current_ms) == HAL_OK);

    for (auto& led : leds)
    {
      led.toggle();
      delay_ms(250U);
    }
  }
}
