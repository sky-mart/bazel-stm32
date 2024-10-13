#pragma once

#include "bsp/spi/spi_device.h"

namespace mart {

class Sdcard : public SpiDevice
{
public:
  static constexpr size_t BLOCK_SIZE = 512U;

  using SpiDevice::SpiDevice;

  int init(Pin clk, Pin mosi, Pin miso, uint32_t spi_alternate);

  int get_blocks_number(uint32_t* number);

  int read_single_block(uint32_t block_num, uint8_t *data);
  int write_single_block(uint32_t block_num, const uint8_t *data);

  // read multiple blocks
  int read_begin(uint32_t block_num);
  int read_data(uint8_t* data);
  int read_end();

  // write multiple blocks
  int write_begin(uint32_t block_num);
  int write_data(const uint8_t* data);
  int write_end();

private:
  int read_r1();
  int read_bytes(uint8_t* data, size_t size);
  int wait_not_busy();
  int wait_data_token(uint8_t token);
};

}
