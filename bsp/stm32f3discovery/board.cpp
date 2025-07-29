#include "bsp/stm32f3discovery/board.h"

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

namespace stm32f3discovery {

mart::PinArray<8U> leds()
{
  return {
    mart::Pin{*GPIOE, GPIO_PIN_8},
    mart::Pin{*GPIOE, GPIO_PIN_9},
    mart::Pin{*GPIOE, GPIO_PIN_10},
    mart::Pin{*GPIOE, GPIO_PIN_11},
    mart::Pin{*GPIOE, GPIO_PIN_12},
    mart::Pin{*GPIOE, GPIO_PIN_13},
    mart::Pin{*GPIOE, GPIO_PIN_14},
    mart::Pin{*GPIOE, GPIO_PIN_15},
  };
}

}
