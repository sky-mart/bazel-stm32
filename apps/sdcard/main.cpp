#include "stm32f3xx_hal.h"
#include "bsp/pin/pin.h"
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

class Gyro
{
public:
  Gyro(SPI_TypeDef* spi, Pin spi_pins, Pin cs_pin) : spi_pins_(spi_pins), cs_pin_(cs_pin)
  {
    hspi_.Instance = spi;
    hspi_.Init.Mode = SPI_MODE_MASTER;
    hspi_.Init.Direction = SPI_DIRECTION_2LINES;
    hspi_.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi_.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi_.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi_.Init.NSS = SPI_NSS_SOFT;
    hspi_.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi_.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi_.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi_.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi_.Init.CRCPolynomial = 7;
    hspi_.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi_.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  }

  HAL_StatusTypeDef init(uint32_t spi_alternate)
  {
    spi_pins_.init_alternate(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, spi_alternate);
    cs_pin_.init_as_led(GPIO_PULLUP);

    return HAL_SPI_Init(&hspi_);
  }

  uint8_t read_reg(uint8_t data)
  {
    data |= READ_CMD;

    cs_pin_.clear();

    if (HAL_SPI_Transmit(&hspi_, &data, sizeof(data), TIMEOUT) != HAL_OK)
    {
      error_handler();
    }

    if (HAL_SPI_Receive(&hspi_, &data, sizeof(data), TIMEOUT) != HAL_OK)
    {
      error_handler();
    }

    cs_pin_.set();

    return data;
  }

private:
  static constexpr uint8_t READ_CMD = 0x80U;
  static constexpr int TIMEOUT = 1000;

  SPI_HandleTypeDef hspi_;
  Pin spi_pins_;
  Pin cs_pin_;
};

int main()
{
  HAL_Init();

  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  Gyro gyro{SPI1, Pin{GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7}, {GPIOE, GPIO_PIN_3}};

  if (gyro.init(GPIO_AF5_SPI1) != HAL_OK)
  {
    error_handler();
  }

  while (true)
  {
    const uint8_t whoami = gyro.read_reg(0xF);
    if (whoami != 0xD4)
    {
      error_handler();
    }

    delay_ms(500);
  }
}
