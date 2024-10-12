#pragma once

#include "bsp/led/led_array.h"
#include "stm32f3xx.h"

namespace stm32f3discovery {

mart::LedArray<8U> leds()
{
  return {
    mart::Led{GPIOE, GPIO_PIN_8},
    mart::Led{GPIOE, GPIO_PIN_9},
    mart::Led{GPIOE, GPIO_PIN_10},
    mart::Led{GPIOE, GPIO_PIN_11},
    mart::Led{GPIOE, GPIO_PIN_12},
    mart::Led{GPIOE, GPIO_PIN_13},
    mart::Led{GPIOE, GPIO_PIN_14},
    mart::Led{GPIOE, GPIO_PIN_15},
  };
}

}
