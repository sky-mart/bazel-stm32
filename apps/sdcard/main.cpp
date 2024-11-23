#include "fatfs/ff.h"
#include "stm32f3xx_hal.h"
#include "bsp/ili9341/ili9341.h"
#include "bsp/sdcard/sdcard.h"
#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/stm32f3discovery/board.h"
#include "bsp/utility/utility.h"
#include "apps/sdcard/wav_header.h"
#include "ff.h"

#include <cstdio>
#include <cstring>

using namespace mart;

Sdcard sdcard{stm32f3discovery::hspi1, Pin{GPIOE, GPIO_PIN_5}};
FATFS fs;
I2S_HandleTypeDef hi2s;
Ili9341 display{stm32f3discovery::hspi1, Pin{GPIOE, GPIO_PIN_4}, Pin{GPIOE, GPIO_PIN_2}, Pin{GPIOE, GPIO_PIN_3}};

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

volatile bool should_play = false;
volatile bool end_of_file_reached = false;
volatile bool read_next_chunk = false;
volatile uint16_t* signal_play_buff = NULL;
volatile uint16_t* signal_read_buff = NULL;
volatile uint16_t signal_buff1[4096];
volatile uint16_t signal_buff2[4096];

extern "C" {
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (!should_play || end_of_file_reached)
        return;

    volatile uint16_t* temp = signal_play_buff;
    signal_play_buff = signal_read_buff;
    signal_read_buff = temp;

    int nsamples = sizeof(signal_buff1) / sizeof(signal_buff1[0]);
    HAL_I2S_Transmit_IT(hi2s, (uint16_t*)signal_play_buff, nsamples);
    read_next_chunk = true;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_0)
  {
    if (should_play)
    {
      should_play = false;
    }
    else
    {
      should_play = true;
      int nsamples = sizeof(signal_buff1) / sizeof(signal_buff1[0]);
      HAL_I2S_Transmit_IT(&hi2s, (uint16_t*)signal_play_buff, nsamples);
      read_next_chunk = true;
    }
  }
}
}

int play_wav(const char* filename, I2S_HandleTypeDef *hi2s)
{
  FIL file;
  FRESULT fat_res = f_open(&file, filename, FA_READ);
  assert(fat_res == FR_OK);

  UINT bytes_read;
  WavHeader wav_header;
  fat_res = f_read(&file, &wav_header, sizeof(wav_header), &bytes_read);
  assert(fat_res == FR_OK);

  uint32_t data_size = wav_header.subchunk2Size;

  fat_res = f_read(&file, (uint8_t*)signal_buff1, sizeof(signal_buff1), &bytes_read);
  assert(fat_res == FR_OK);

  fat_res = f_read(&file, (uint8_t*)signal_buff2, sizeof(signal_buff2), &bytes_read);
  assert(fat_res == FR_OK);

  should_play = true;
  read_next_chunk = false;
  end_of_file_reached = false;
  signal_play_buff = signal_buff1;
  signal_read_buff = signal_buff2;

  HAL_StatusTypeDef hal_res;
  int nsamples = sizeof(signal_buff1) / sizeof(signal_buff1[0]);
  hal_res = HAL_I2S_Transmit_IT(hi2s, (uint16_t*)signal_play_buff, nsamples);
  assert(hal_res == HAL_OK);

  while (data_size >= sizeof(signal_buff1)) {
    if(!read_next_chunk) {
      continue;
    }

    read_next_chunk = false;

    fat_res = f_read(&file, (uint8_t*)signal_read_buff, sizeof(signal_buff1), &bytes_read);
    assert(fat_res == FR_OK);

    data_size -= sizeof(signal_buff1);
  }

  end_of_file_reached = true;

  fat_res = f_close(&file);
  assert(fat_res == FR_OK);

  return 0;
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  auto res = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  assert(res == HAL_OK);

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_SYSCLK_DIV1;
  res = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
  assert(res == HAL_OK);
}

void init_i2s()
{
  GPIO_InitTypeDef config;
  config.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
  config.Mode = GPIO_MODE_AF_PP;
  config.Pull = GPIO_NOPULL;
  config.Alternate = GPIO_AF5_I2S;
  HAL_GPIO_Init(GPIOB, &config);

  HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(SPI2_IRQn);

  hi2s.Instance = SPI2;
  hi2s.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s.Init.Standard = I2S_STANDARD_PCM_SHORT;
  hi2s.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s.Init.AudioFreq = I2S_AUDIOFREQ_16K;
  hi2s.Init.CPOL = I2S_CPOL_LOW;
  hi2s.Init.ClockSource = I2S_CLOCK_SYSCLK;
  hi2s.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  auto hal_res = HAL_I2S_Init(&hi2s);
  assert(hal_res == HAL_OK);
}

void init_button()
{
  GPIO_InitTypeDef config;
  config.Pin = GPIO_PIN_0;
  config.Mode = GPIO_MODE_IT_RISING;
  config.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &config);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

int main()
{
  HAL_Init();

  SystemClock_Config();

  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_SPI2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  init_i2s();
  init_button();

  // auto leds = stm32f3discovery::leds();
  // leds.init_as_leds();

  // we create it just to unselect
  stm32f3discovery::Gyroscope gyro;
  gyro.unselect();
  display.unselect();

  const int sdcard_res = sdcard.init(Pin{GPIOA, GPIO_PIN_5}, Pin{GPIOA, GPIO_PIN_6}, Pin{GPIOA, GPIO_PIN_7}, GPIO_AF5_SPI1);
  assert(sdcard_res >= 0);

  display.init();
  display.fill_screen(0);
  delay_ms(100);

  FRESULT fat_res = f_mount(&fs, "", 0);
  assert(fat_res == FR_OK);

  while (true)
  {
    play_wav("imp-max.wav", &hi2s);
    delay_ms(5000);
  }
}
