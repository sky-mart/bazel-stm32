#pragma once

#include "bsp/spi/spi_device.h"

namespace stm32f3discovery {

class Gyroscope : public mart::SpiDevice
{
public:
  Gyroscope();

  HAL_StatusTypeDef init();

  std::optional<uint8_t> read(uint8_t data, int timeout = HAL_MAX_DELAY);
};

}
