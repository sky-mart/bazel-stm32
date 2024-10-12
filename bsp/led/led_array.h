#pragma once

#include "bsp/led/led.h"
#include <array>

namespace mart {

template <size_t N>
class LedArray
{
public:
  template <typename... Args>
  LedArray(Args&&... leds)
    : leds_{ std::forward<Args>(leds)... }  // Forward parameters to construct the array
  {
    static_assert(sizeof...(leds) == N, "Number of leds must match the array size");
  }

  void init()
  {
    for (size_t i = 0; i < N; ++i)
    {
      leds_[i].init();
    }
  }

  size_t count() const { return N; }

  Led& operator[](size_t i) { return leds_[i]; }

private:
  std::array<Led, N> leds_;
};

}
