#include "stm32f3xx_hal.h"

extern I2S_HandleTypeDef hi2s;

extern "C" {

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void SPI2_IRQHandler(void)
{
  HAL_I2S_IRQHandler(&hi2s);
}

void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

}
