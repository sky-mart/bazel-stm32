#include "bsp/pin/pin.h"

namespace mart {

Pin::Pin(GPIO_TypeDef& port, u32 pin) : port_(port), pin_(pin) {}

void Pin::init(u32 mode, u32 pull, u32 speed, u32 alternate)
{
  GPIO_InitTypeDef config;
  config.Pin = pin_;
  config.Mode = mode;
  config.Pull = pull;
  config.Speed = speed;
  config.Alternate = alternate;

  HAL_GPIO_Init(&port_, &config);
}

void Pin::init_as_led(u32 pull)
{
  init(GPIO_MODE_OUTPUT_PP, pull, GPIO_SPEED_FREQ_LOW);
}

void Pin::set() { HAL_GPIO_WritePin(&port_, pin_, GPIO_PIN_SET); }

void Pin::high() { set(); }

void Pin::clear() { HAL_GPIO_WritePin(&port_, pin_, GPIO_PIN_RESET); }

void Pin::low() { clear(); }

void Pin::toggle() { HAL_GPIO_TogglePin(&port_, pin_); }

}
