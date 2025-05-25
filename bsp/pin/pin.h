#pragma once

#include "stm32f3xx_hal_gpio.h"
#include "std_ext/types.h"

namespace mart {

class Pin
{
public:
  Pin(GPIO_TypeDef& port, uint32_t pin);

  void init(u32 mode, u32 pull, u32 speed, u32 alternate = 0);
  void init_as_led(u32 pull = GPIO_PULLDOWN);

  void set();
  void high();
  void clear();
  void low();
  void toggle();

private:
  GPIO_TypeDef& port_;
  const u32 pin_;
};

}
