#include "bsp/spi/spi_device.h"
#include "bsp/spi/chip_select.h"

namespace mart {

SpiDevice::SpiDevice(SPI_TypeDef *instance, Pin cs_pin) : cs_pin_(cs_pin)
{
  hspi_.Instance = instance;
}

HAL_StatusTypeDef SpiDevice::init(const SPI_InitTypeDef &config)
{
  cs_pin_.init_as_led(GPIO_PULLUP);
  hspi_.Init = config;
  return HAL_SPI_Init(&hspi_);
}

void SpiDevice::select()
{
  cs_pin_.low();
}

void SpiDevice::unselect()
{
  cs_pin_.high();
}

std::optional<uint8_t> SpiDevice::read(uint8_t data, int timeout)
{
  ChipSelect cs{cs_pin_};

  if (HAL_SPI_Transmit(&hspi_, &data, sizeof(data), timeout) != HAL_OK)
  {
    return std::nullopt;
  }

  if (HAL_SPI_Receive(&hspi_, &data, sizeof(data), timeout) != HAL_OK)
  {
    return std::nullopt;
  }

  return data;
}

}
