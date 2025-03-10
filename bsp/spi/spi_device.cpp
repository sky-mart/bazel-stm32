#include "bsp/spi/spi_device.h"
#include "bsp/spi/chip_select.h"

namespace mart {

SpiDevice::SpiDevice(SPI_HandleTypeDef& hspi, Pin cs_pin) : hspi_(hspi), cs_pin_(cs_pin)
{
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

ChipSelect SpiDevice::make_chip_select()
{
  return ChipSelect{cs_pin_};
}

std::optional<uint8_t> SpiDevice::read(uint8_t data, int timeout)
{
  const auto cs = make_chip_select();

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

HAL_StatusTypeDef SpiDevice::transmit(uint8_t data, int timeout)
{
  return HAL_SPI_Transmit(&hspi_, &data, sizeof(data), timeout);
}

HAL_StatusTypeDef SpiDevice::transmit(uint8_t* data, size_t size, int timeout)
{
  return HAL_SPI_Transmit(&hspi_, data, size, timeout);
}

}
