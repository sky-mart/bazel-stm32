#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/spi/spi_device.h"
#include "bsp/stm32f3discovery/board.h"

using namespace mart;

namespace stm32f3discovery {

constexpr uint8_t READ_CMD = 0x80U;

Gyroscope::Gyroscope() : SpiDevice(hspi1, Pin{GPIOE, GPIO_PIN_3})
{
}

HAL_StatusTypeDef Gyroscope::init()
{
  Pin spi_pins{GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7};
  spi_pins.init_alternate(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF5_SPI1);

  SPI_InitTypeDef config;
  config.Mode = SPI_MODE_MASTER;
  config.Direction = SPI_DIRECTION_2LINES;
  config.DataSize = SPI_DATASIZE_8BIT;
  config.CLKPolarity = SPI_POLARITY_HIGH;
  config.CLKPhase = SPI_PHASE_2EDGE;
  config.NSS = SPI_NSS_SOFT;
  config.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  config.FirstBit = SPI_FIRSTBIT_MSB;
  config.TIMode = SPI_TIMODE_DISABLE;
  config.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  config.CRCPolynomial = 7;
  config.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  config.NSSPMode = SPI_NSS_PULSE_DISABLE;

  return SpiDevice::init(config);
}

std::optional<uint8_t> Gyroscope::read(Register reg, int timeout)
{
  return SpiDevice::read(static_cast<uint8_t>(reg) | READ_CMD, timeout);
}


}
