#pragma once

#include "bsp/pin/pin.h"
#include "stm32f3xx_hal_dma.h"
#include "stm32f3xx_hal_uart.h"
#include "std_ext/types.h"

namespace mart {

class UartDevice
{
public:
  explicit UartDevice(USART_TypeDef& uart);

  HAL_StatusTypeDef init(Pin uart_pins, u32 baudrate);

  HAL_StatusTypeDef printf(const char* fmt, ...);

protected:
  UART_HandleTypeDef handle_{};
};

}
