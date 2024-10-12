#include "stm32f3xx.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio.h"

GPIO_TypeDef * const led_port = GPIOE;
const auto led_pin = GPIO_PIN_8;

void delay_ms(uint32_t ms)
{
  HAL_Delay(ms / uwTickFreq);
}

int main()
{
  HAL_Init();

  RCC->AHBENR |= RCC_AHBENR_GPIOEEN; // Clock

  GPIO_InitTypeDef led_config;
  led_config.Pin = led_pin;
  led_config.Mode = GPIO_MODE_OUTPUT_PP;
  led_config.Pull = GPIO_PULLDOWN;
  led_config.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(led_port, &led_config);

  while (true)
  {
    HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);
    delay_ms(500U);
    HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_RESET);
    delay_ms(500U);
  }
}

extern "C" {
void SysTick_Handler(void)
{
  HAL_IncTick();
}
}
