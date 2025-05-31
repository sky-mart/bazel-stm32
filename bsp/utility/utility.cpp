#include "bsp/utility/utility.h"
#include "stm32f3xx_hal.h"

namespace mart {

void delay_ms(u32 ms)
{
  HAL_Delay(ms / uwTickFreq);
}

u32 ms_since_startup()
{
  return HAL_GetTick() * uwTickFreq;
}

}
