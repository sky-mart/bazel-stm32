#pragma once

#include "stm32f3xx_hal_gpio.h"

namespace mart {

class Pin
{
public:
  Pin(GPIO_TypeDef& port, uint32_t pin);

  void init(uint32_t mode, uint32_t pull, uint32_t speed, uint32_t alternate = 0);
  void init_as_led(uint32_t pull = GPIO_PULLDOWN);

  void set();
  void high();
  void clear();
  void low();
  void toggle();

private:
  GPIO_TypeDef& port_;
  const uint32_t pin_;
};

}
