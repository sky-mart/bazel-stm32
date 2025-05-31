#pragma once

#include "bsp/pin/pin.h"
#include "bsp/spi/chip_select.h"
#include "std_ext/types.h"
#include "stm32f3xx_hal_dma.h"
#include "stm32f3xx_hal_spi.h"

#include <optional>

namespace mart {

class SpiDevice
{
public:
  SpiDevice(SPI_TypeDef& spi, Pin cs_pin);

  HAL_StatusTypeDef init(const SPI_InitTypeDef& config);

  void select();

  void unselect();

  std::optional<u8> read(u8 addr, u32 timeout = HAL_MAX_DELAY);

  HAL_StatusTypeDef transmit(u8 data, u32 timeout = HAL_MAX_DELAY);
  HAL_StatusTypeDef transmit(u8* data, size_t size, u32 timeout = HAL_MAX_DELAY);

protected:
  ChipSelect make_chip_select();

  SPI_HandleTypeDef handle_{};
  Pin cs_pin_;
};

}
