#include "bsp/utility/utility.h"
#include "stm32f3xx_hal.h"

namespace mart {

void delay_ms(uint32_t ms)
{
  HAL_Delay(ms / uwTickFreq);
}

}
