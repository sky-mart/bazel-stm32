#pragma once

#include "bsp/pin/pin.h"

namespace mart {

class ChipSelect
{
public:
  explicit ChipSelect(Pin cs_pin) : cs_pin_(cs_pin)
  {
    cs_pin_.low();
  }

  ~ChipSelect()
  {
    cs_pin_.high();
  }

  ChipSelect(const ChipSelect& other) = delete;
  ChipSelect& operator=(const ChipSelect& other) = delete;
  ChipSelect(ChipSelect&& other) : cs_pin_(other.cs_pin_) {}
  ChipSelect& operator=(ChipSelect&& other) = delete;

private:
  Pin cs_pin_;
};

}
