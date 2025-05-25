#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_rcc.h"
#include "bsp/stm32f3discovery/board.h"
#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/utility/utility.h"

using namespace mart;

auto leds = stm32f3discovery::leds();
stm32f3discovery::Gyroscope gyro{};

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

  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  leds.init_as_leds();
  gyro.init();

  while (true)
  {
    const auto whoami = gyro.read(stm32f3discovery::Gyroscope::Register::WHO_AM_I);
    assert(whoami.has_value());
    assert(whoami.value() == 0xD4U);

    for (auto& led : leds)
    {
      led.toggle();
      delay_ms(250U);
    }
  }
}
