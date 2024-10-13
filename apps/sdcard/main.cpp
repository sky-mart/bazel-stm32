#include "stm32f3xx_hal.h"
#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/utility/utility.h"

using namespace mart;

void error_handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

int main()
{
  HAL_Init();

  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  stm32f3discovery::Gyroscope gyro;

  if (gyro.init() != HAL_OK)
  {
    error_handler();
  }

  while (true)
  {
    const auto maybe_whoami = gyro.read(0xF);
    if (!maybe_whoami.has_value())
    {
      error_handler();
    }
    else if (maybe_whoami.value() != 0xD4)
    {
      error_handler();
    }

    delay_ms(500);
  }
}
