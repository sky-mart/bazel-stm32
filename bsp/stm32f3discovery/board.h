#pragma once

#include "bsp/pin/pin_array.h"
#include "stm32f3xx_hal.h"

namespace stm32f3discovery {

mart::PinArray<8U> leds();

mart::Pin spi_pins(SPI_TypeDef* spi);

}
