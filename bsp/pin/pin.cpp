#include "bsp/pin/pin.h"

namespace mart {

Pin::Pin(GPIO_TypeDef* port, uint32_t pin) : port_(port), pin_(pin) {}

void Pin::init(uint32_t mode, uint32_t pull, uint32_t speed)
{
  GPIO_InitTypeDef config;
  config.Pin = pin_;
  config.Mode = mode;
  config.Pull = pull;
  config.Speed = speed;

  HAL_GPIO_Init(port_, &config);
}

void Pin::init_as_led(uint32_t pull)
{
  init(GPIO_MODE_OUTPUT_PP, pull, GPIO_SPEED_FREQ_LOW);
}

void Pin::init_alternate(uint32_t mode, uint32_t pull, uint32_t speed,
                         uint32_t alternate)
{
  GPIO_InitTypeDef config;
  config.Pin = pin_;
  config.Mode = mode;
  config.Pull = pull;
  config.Speed = speed;
  config.Alternate = alternate;

  HAL_GPIO_Init(port_, &config);
}

void Pin::set() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET); }

void Pin::clear() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET); }

void Pin::toggle() { HAL_GPIO_TogglePin(port_, pin_); }

}
