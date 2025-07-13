#include "bsp/stm32f3discovery/gyroscope.h"
#include "bsp/spi/spi_device.h"
#include "bsp/stm32f3discovery/board.h"

using namespace mart;

namespace stm32f3discovery {

constexpr u8 READ_CMD = 0x80U;

Gyroscope::Gyroscope(SPI_TypeDef& spi, mart::Pin cs_pin) : SpiDevice(spi, std::move(cs_pin))
{
}

HAL_StatusTypeDef Gyroscope::init()
{
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

std::optional<u8> Gyroscope::read(Register reg, i32 timeout)
{
  return SpiDevice::read(static_cast<u8>(reg) | READ_CMD, timeout);
}


}
