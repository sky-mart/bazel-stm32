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

private:
  Pin cs_pin_;
};

}
