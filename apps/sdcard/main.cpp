#include "stm32f3xx_hal.h"
#include "bsp/sdcard/sdcard.h"
#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/utility/utility.h"

#include <cstdio>
#include <cstring>

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
  gyro.unselect();

  Sdcard sdcard{SPI1, Pin{GPIOE, GPIO_PIN_5}};

  int res = sdcard.init(Pin{GPIOA, GPIO_PIN_5}, Pin{GPIOA, GPIO_PIN_6}, Pin{GPIOA, GPIO_PIN_7}, GPIO_AF5_SPI1);
  if (res < 0)
  {
    error_handler();
  }

  uint32_t blocks_number;
  res = sdcard.get_blocks_number(&blocks_number);
  if (res < 0)
  {
    error_handler();
  }

  const uint32_t start_block_addr = 0x00ABCD;
  uint32_t block_addr = start_block_addr;
  uint8_t block[512];

  snprintf((char*)block, sizeof(block), "0x%08X", (int)block_addr);

  res = sdcard.write_single_block(block_addr, block);
  if (res < 0)
  {
    error_handler();
  }

  memset(block, 0, sizeof(block));

  res = sdcard.read_single_block(block_addr, block);
  if (res < 0)
  {
    error_handler();
  }

  block_addr = start_block_addr + 1;
  res = sdcard.write_begin(block_addr);
  if (res < 0)
  {
    error_handler();
  }

  for(int i = 0; i < 3; i++)
  {
    snprintf((char*)block, sizeof(block), "0x%08X", (int)block_addr);

    res = sdcard.write_data(block);
    if (res < 0)
    {
      error_handler();
    }

    block_addr++;
  }

  res = sdcard.write_end();
  if (res < 0)
  {
    error_handler();
  }

  block_addr = start_block_addr + 1;
  res = sdcard.read_begin(block_addr);
  if (res < 0)
  {
    error_handler();
  }

  for(int i = 0; i < 3; i++)
  {
    res = sdcard.read_data(block);
    if (res < 0)
    {
      error_handler();
    }
  }

  res = sdcard.read_end();
  if (res < 0)
  {
    error_handler();
  }

  while (true)
  {
  }
}
