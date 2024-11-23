#pragma once

#include "bsp/spi/spi_device.h"

namespace mart {

class Ili9341 : public SpiDevice
{
public:
  Ili9341(SPI_HandleTypeDef& hspi, Pin cs_pin, Pin dc_pin, Pin res_pin);

  void init();
  void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
  void fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
  void fill_screen(uint16_t color);
  void draw_image(uint16_t, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
  void invert_colors(bool invert);

private:
  void reset();
  void write_command(uint8_t cmd);
  void write_data(uint8_t* data, size_t size);
  void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

  Pin dc_pin_;
  Pin res_pin_;
  uint16_t width_;
  uint16_t height_;
};

}
