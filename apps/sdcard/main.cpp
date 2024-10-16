#include "stm32f3xx_hal.h"
#include "bsp/sdcard/sdcard.h"
#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/stm32f3discovery/board.h"
#include "bsp/utility/utility.h"
#include "ff.h"

#include <cstdio>
#include <cstring>

using namespace mart;

Sdcard sdcard{SPI1, Pin{GPIOE, GPIO_PIN_5}};
FATFS fs;

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

  auto leds = stm32f3discovery::leds();
  leds.init_as_leds();

  // we create it just to unselect
  stm32f3discovery::Gyroscope gyro;
  gyro.unselect();

  // Sdcard sdcard{SPI1, Pin{GPIOE, GPIO_PIN_5}};

  const int sdcard_res = sdcard.init(Pin{GPIOA, GPIO_PIN_5}, Pin{GPIOA, GPIO_PIN_6}, Pin{GPIOA, GPIO_PIN_7}, GPIO_AF5_SPI1);
  if (sdcard_res < 0)
  {
    error_handler();
  }

  FRESULT fat_res = f_mount(&fs, "", 0);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  uint32_t free_clust;
  FATFS* fs_ptr = &fs;
  fat_res = f_getfree("", &free_clust, &fs_ptr); // Warning! This fills fs.n_fatent and fs.csize!
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  uint32_t totalBlocks = (fs.n_fatent - 2) * fs.csize;
  uint32_t freeBlocks = free_clust * fs.csize;

  DIR dir;
  fat_res = f_opendir(&dir, "/");
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  FILINFO fileInfo;
  uint32_t totalFiles = 0;
  uint32_t totalDirs = 0;
  for(;;) {
    fat_res = f_readdir(&dir, &fileInfo);
    if ((fat_res != FR_OK) || (fileInfo.fname[0] == '\0')) {
      break;
    }

    if(fileInfo.fattrib & AM_DIR) {
      totalDirs++;
    } else {
      totalFiles++;
    }
  }

  fat_res = f_closedir(&dir);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  char writeBuff[128];
  snprintf(writeBuff, sizeof(writeBuff), "Total blocks: %lu (%lu Mb); Free blocks: %lu (%lu Mb)\r\n",
           totalBlocks, totalBlocks / 2000,
           freeBlocks, freeBlocks / 2000);

  FIL logFile;
  fat_res = f_open(&logFile, "log5.txt", FA_OPEN_APPEND | FA_WRITE);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  unsigned int bytesToWrite = strlen(writeBuff);
  unsigned int bytesWritten;
  fat_res = f_write(&logFile, writeBuff, bytesToWrite, &bytesWritten);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  fat_res = f_close(&logFile);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  FIL msgFile;
  fat_res = f_open(&msgFile, "log5.txt", FA_READ);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  char readBuff[128];
  unsigned int bytesRead;
  fat_res = f_read(&msgFile, readBuff, sizeof(readBuff)-1, &bytesRead);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  readBuff[bytesRead] = '\0';

  fat_res = f_close(&msgFile);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  // Unmount
  fat_res = f_mount(NULL, "", 0);
  if (fat_res != FR_OK)
  {
    error_handler();
  }

  while (true)
  {
    for (size_t i = 0; i < leds.count(); ++i)
    {
      leds[i].toggle();
      delay_ms(250U);
    }
  }
}
