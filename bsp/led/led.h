#include "stm32f3xx_hal_gpio.h"

namespace mart {

class Led
{
public:
  Led(GPIO_TypeDef* port, uint32_t pin);

  void init();
  void set();
  void clear();

private:
  GPIO_TypeDef * const port_;
  const uint32_t pin_;
};

}
