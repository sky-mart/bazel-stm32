#pragma once

#include "bsp/spi/spi_device.h"
#include "std_ext/types.h"

namespace stm32f3discovery {

class Gyroscope : public mart::SpiDevice
{
public:
  enum class Register : u8
  {
    WHO_AM_I      = 0x0FU,
    CTRL_REG1     = 0x20U,
    CTRL_REG2     = 0x21U,
    CTRL_REG3     = 0x22U,
    CTRL_REG4     = 0x23U,
    CTRL_REG5     = 0x24U,
    REFERENCE     = 0x25U,
    OUT_TEMP      = 0x26U,
    STATUS_REG    = 0x27U,
    OUT_X_L       = 0x28U,
    OUT_X_H       = 0x29U,
    OUT_Y_L       = 0x2AU,
    OUT_Y_H       = 0x2BU,
    OUT_Z_L       = 0x2CU,
    OUT_Z_H       = 0x2DU,
    FIFO_CTRL_REG = 0x2EU,
    FIFO_SRC_REG  = 0x2FU,
    INT1_CFG      = 0x30U,
    INT1_SRC      = 0x31U,
    INT1_TSH_XH   = 0x32U,
    INT1_TSH_XL   = 0x33U,
    INT1_TSH_YH   = 0x34U,
    INT1_TSH_YL   = 0x35U,
    INT1_TSH_ZH   = 0x36U,
    INT1_TSH_ZL   = 0x37U,
    INT1_DURATION = 0x38U,
  };

  Gyroscope();

  HAL_StatusTypeDef init();

  std::optional<u8> read(Register reg, i32 timeout = HAL_MAX_DELAY);
};

}
