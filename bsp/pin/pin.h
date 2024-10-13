#include "stm32f3xx_hal_gpio.h"

namespace mart {

class Pin
{
public:
  Pin(GPIO_TypeDef* port, uint32_t pin);

  void init();
  void set();
  void clear();
  void toggle();

private:
  GPIO_TypeDef * const port_;
  const uint32_t pin_;
};

}
