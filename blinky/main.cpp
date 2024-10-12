#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_rcc.h"
#include "bsp/led/led.h"
#include "bsp/utility/utility.h"

using namespace mart;

Led led{GPIOE, GPIO_PIN_11};

int main()
{
  HAL_Init();

  __HAL_RCC_GPIOE_CLK_ENABLE();

  led.init();

  while (true)
  {
    led.set();
    delay_ms(500U);
    led.clear();
    delay_ms(500U);
  }
}
