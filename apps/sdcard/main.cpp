#include "fatfs/ff.h"
#include "stm32f3xx_hal.h"
#include "bsp/sdcard/sdcard.h"
#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/stm32f3discovery/board.h"
#include "bsp/utility/utility.h"
#include "apps/sdcard/wav_header.h"
#include "ff.h"

#include <cstdio>
#include <cstring>

using namespace mart;

Sdcard sdcard{SPI1, Pin{GPIOE, GPIO_PIN_5}};
FATFS fs;

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

  auto leds = stm32f3discovery::leds();
  leds.init_as_leds();

  // we create it just to unselect
  stm32f3discovery::Gyroscope gyro;
  gyro.unselect();

  const int sdcard_res = sdcard.init(Pin{GPIOA, GPIO_PIN_5}, Pin{GPIOA, GPIO_PIN_6}, Pin{GPIOA, GPIO_PIN_7}, GPIO_AF5_SPI1);
  assert(sdcard_res >= 0);

  FRESULT fat_res = f_mount(&fs, "", 0);
  assert(fat_res == FR_OK);

  FIL file;
  fat_res = f_open(&file, "imp-max.wav", FA_READ);
  assert(fat_res == FR_OK);

  UINT bytes_read;
  WavHeader wav_header;
  fat_res = f_read(&file, &wav_header, sizeof(wav_header), &bytes_read);
  assert(fat_res == FR_OK);

  fat_res = f_close(&file);
  assert(fat_res == FR_OK);

  // Unmount
  // fat_res = f_mount(NULL, "", 0);
  // assert(fat_res == FR_OK);

  while (true)
  {
    for (size_t i = 0; i < leds.count(); ++i)
    {
      leds[i].toggle();
      delay_ms(250U);
    }
  }
}
