#pragma once

#include "bsp/pin/pin.h"
#include <array>

namespace mart {

template <size_t N>
class PinArray
{
public:
  template <typename... Args>
  PinArray(Args&&... leds)
    : leds_{ std::forward<Args>(leds)... }  // Forward parameters to construct the array
  {
    static_assert(sizeof...(leds) == N, "Number of leds must match the array size");
  }

  void init_as_leds()
  {
    for (auto& led : leds_)
    {
      led.init_as_led();
    }
  }

  size_t count() const { return N; }

  Pin& operator[](size_t i) { return leds_[i]; }

  auto begin() { return leds_.begin(); }

  auto end() { return leds_.end(); }

private:
  std::array<Pin, N> leds_;
};

}
