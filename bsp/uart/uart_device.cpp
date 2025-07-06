#include "bsp/uart/uart_device.h"
#include <cstdarg>
#include <cstdio>

namespace mart {

UartDevice::UartDevice(USART_TypeDef &uart) { handle_.Instance = &uart; }

HAL_StatusTypeDef UartDevice::init(u32 baudrate)
{
  handle_.Init.BaudRate = baudrate;
  handle_.Init.WordLength = UART_WORDLENGTH_8B;
  handle_.Init.StopBits = UART_STOPBITS_1;
  handle_.Init.Parity = UART_PARITY_NONE;
  handle_.Init.Mode = UART_MODE_TX_RX;
  handle_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  handle_.Init.OverSampling = UART_OVERSAMPLING_16;
  handle_.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  handle_.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  return HAL_UART_Init(&handle_);
}

HAL_StatusTypeDef UartDevice::printf(const char* fmt, ...)
{
  static char buf[100];
  va_list args;

  va_start(args, fmt);
  int len = vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  if (len > 0 && static_cast<size_t>(len) < sizeof(buf))
  {
    return HAL_UART_Transmit(&handle_, reinterpret_cast<u8*>(&buf[0]), len, HAL_MAX_DELAY);
  }
  return HAL_ERROR;
}

}
