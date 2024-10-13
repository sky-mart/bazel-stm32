#include "bsp/pin/pin.h"

namespace mart {

Pin::Pin(GPIO_TypeDef* port, uint32_t pin) : port_(port), pin_(pin) {}

void Pin::init()
{
  GPIO_InitTypeDef pin_config;
  pin_config.Pin = pin_;
  pin_config.Mode = GPIO_MODE_OUTPUT_PP;
  pin_config.Pull = GPIO_PULLDOWN;
  pin_config.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(port_, &pin_config);
}

void Pin::set() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET); }

void Pin::clear() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET); }

void Pin::toggle() { HAL_GPIO_TogglePin(port_, pin_); }

}
