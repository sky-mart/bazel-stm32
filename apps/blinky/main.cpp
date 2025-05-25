#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_rcc.h"
#include "bsp/stm32f3discovery/board.h"
#include "bsp/utility/utility.h"

using namespace mart;

auto leds = stm32f3discovery::leds();

int main()
{
  HAL_Init();

  __HAL_RCC_GPIOE_CLK_ENABLE();

  leds.init_as_leds();

  while (true)
  {
    for (auto& led : leds)
    {
      led.toggle();
      delay_ms(250U);
    }
  }
}
