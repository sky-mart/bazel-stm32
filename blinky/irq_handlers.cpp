#include "stm32f3xx_hal.h"

extern "C" {

void SysTick_Handler(void)
{
  HAL_IncTick();
}

}
