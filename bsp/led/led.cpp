#include "bsp/led/led.h"

namespace mart {

Led::Led(GPIO_TypeDef* port, uint32_t pin) : port_(port), pin_(pin) {}

void Led::init()
{
  GPIO_InitTypeDef led_config;
  led_config.Pin = pin_;
  led_config.Mode = GPIO_MODE_OUTPUT_PP;
  led_config.Pull = GPIO_PULLDOWN;
  led_config.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(port_, &led_config);
}

void Led::set() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET); }

void Led::clear() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET); }

void Led::toggle() { HAL_GPIO_TogglePin(port_, pin_); }

}
