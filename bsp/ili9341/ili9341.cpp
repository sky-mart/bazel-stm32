#include "bsp/ili9341/ili9341.h"
#include "bsp/utility/utility.h"
#include <cstdint>

namespace mart {

Ili9341::Ili9341(SPI_HandleTypeDef &hspi, Pin cs_pin, Pin dc_pin, Pin res_pin)
  : SpiDevice(hspi, cs_pin), dc_pin_(dc_pin), res_pin_(res_pin)
{
}

void Ili9341::reset()
{
  res_pin_.clear();
  delay_ms(5);
  res_pin_.set();
}

void Ili9341::write_command(uint8_t cmd)
{
  dc_pin_.clear();
  transmit(cmd);
}

void Ili9341::write_data(uint8_t* data, size_t size)
{
  dc_pin_.set();

  while (size > 0)
  {
    const uint16_t chunk_size = size > 32768U ? 32768U : size;
    transmit(data, chunk_size);
    data += chunk_size;
    size -= chunk_size;
  }
}

void Ili9341::set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  // COLUMN_ADDRESS_SET
  write_command(0x2A);
  {
    uint8_t data[] = {static_cast<uint8_t>((x0 >> 8U) & 0xFFU), static_cast<uint8_t>(x0 & 0xFFU),
                      static_cast<uint8_t>((x1 >> 8U) & 0xFFU), static_cast<uint8_t>(x1 & 0xFFU)};
    write_data(data, sizeof(data));
  }

  // ROW_ADDRESS_SET
  write_command(0x2B);
  {
    uint8_t data[] = {static_cast<uint8_t>((y0 >> 8U) & 0xFFU), static_cast<uint8_t>(y0 & 0xFFU),
                      static_cast<uint8_t>((y1 >> 8U) & 0xFFU), static_cast<uint8_t>(y1 & 0xFFU)};
    write_data(data, sizeof(data));
  }
  // WRITE_TO_RAM
  write_command(0x2C);
}

void Ili9341::init()
{
  cs_pin_.init_as_led();
  dc_pin_.init_as_led();
  res_pin_.init_as_led();

  const auto cs = make_chip_select();

  reset();

  // SOFTWARE_RESET
  write_command(0x01);
  delay_ms(1000);

  // POWER_CONTROL_A
  write_command(0xCB);
  {
    uint8_t data[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
    write_data(data, sizeof(data));
  }

  // POWER_CONTROL_B
  write_command(0xCF);
  {
    uint8_t data[] = {0x00, 0xC1, 0x30};
    write_data(data, sizeof(data));
  }

  // DRIVER TIMING CONTROL A
  write_command(0xE8);
  {
    uint8_t data[] = { 0x85, 0x00, 0x78 };
    write_data(data, sizeof(data));
  }

  // DRIVER TIMING CONTROL B
  write_command(0xEA);
  {
    uint8_t data[] = { 0x00, 0x00 };
    write_data(data, sizeof(data));
  }

  // POWER ON SEQUENCE CONTROL
  write_command(0xED);
  {
    uint8_t data[] = { 0x64, 0x03, 0x12, 0x81 };
    write_data(data, sizeof(data));
  }

  // PUMP RATIO CONTROL
  write_command(0xF7);
  {
    uint8_t data[] = { 0x20 };
    write_data(data, sizeof(data));
  }

  // POWER CONTROL,VRH[5:0]
  write_command(0xC0);
  {
    uint8_t data[] = { 0x23 };
    write_data(data, sizeof(data));
  }

  // POWER CONTROL,SAP[2:0];BT[3:0]
  write_command(0xC1);
  {
    uint8_t data[] = { 0x10 };
    write_data(data, sizeof(data));
  }

  // VCM CONTROL
  write_command(0xC5);
  {
    uint8_t data[] = { 0x3E, 0x28 };
    write_data(data, sizeof(data));
  }

  // VCM CONTROL 2
  write_command(0xC7);
  {
    uint8_t data[] = { 0x86 };
    write_data(data, sizeof(data));
  }

  // MEMORY ACCESS CONTROL
  write_command(0x36);
  {
    uint8_t data[] = { 0x48 };
    write_data(data, sizeof(data));
  }

  // PIXEL FORMAT
  write_command(0x3A);
  {
    uint8_t data[] = { 0x55 };
    write_data(data, sizeof(data));
  }

  // FRAME RATIO CONTROL, STANDARD RGB COLOR
  write_command(0xB1);
  {
    uint8_t data[] = { 0x00, 0x18 };
    write_data(data, sizeof(data));
  }

  // DISPLAY FUNCTION CONTROL
  write_command(0xB6);
  {
    uint8_t data[] = { 0x08, 0x82, 0x27 };
    write_data(data, sizeof(data));
  }

  // 3GAMMA FUNCTION DISABLE
  write_command(0xF2);
  {
    uint8_t data[] = { 0x00 };
    write_data(data, sizeof(data));
  }

  // GAMMA CURVE SELECTED
  write_command(0x26);
  {
    uint8_t data[] = { 0x01 };
    write_data(data, sizeof(data));
  }

  // POSITIVE GAMMA CORRECTION
  write_command(0xE0);
  {
    uint8_t data[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                       0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
    write_data(data, sizeof(data));
  }

  // NEGATIVE GAMMA CORRECTION
  write_command(0xE1);
  {
    uint8_t data[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                       0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
    write_data(data, sizeof(data));
  }

  // EXIT SLEEP
  write_command(0x11);
  delay_ms(200);

  // TURN ON DISPLAY
  write_command(0x29);

  // MADCTL
  write_command(0x36);
  {
    uint8_t data[] = { 0x40 | 0x08 };
    write_data(data, sizeof(data));
  }
}

void Ili9341::draw_pixel(const uint16_t x, const uint16_t y, const uint16_t color)
{
  if (x >= width_ || y >= height_)
  {
    return;
  }

  const auto cs = make_chip_select();

  set_address_window(x, y, x + 1, y + 1);
  uint8_t data[] = {static_cast<uint8_t>(color >> 8U), static_cast<uint8_t>(color & 0xFFU)};
  write_data(data, sizeof(data));
}

void Ili9341::fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  if (x >= width_ || y >= height_)
  {
    return;
  }
  if ((x + w) > width_)
  {
    w = width_ - x;
  }
  if ((y + h) > height_)
  {
    h = height_ - y;
  }

  const auto cs = make_chip_select();
  set_address_window(x, y, x + w - 1, y + h - 1);

  uint8_t data[] = {static_cast<uint8_t>(color >> 8U), static_cast<uint8_t>(color & 0xFFU)};
  dc_pin_.set();
  for (y = h; y > 0; y --)
  {
    for (x = w; x > 0; x--)
    {
      transmit(data, sizeof(data));
    }
  }
}

void Ili9341::fill_screen(const uint16_t color)
{
  fill_rectangle(0, 0, width_, height_, color);
}

void Ili9341::draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data)
{
  if (x >= width_ || y >= height_)
  {
    return;
  }
  if ((x + w) > width_)
  {
    return;
  }
  if ((y + h) > height_)
  {
    return;
  }

  const auto cs = make_chip_select();
  set_address_window(x, y, x + w - 1, y + h - 1);
  write_data((uint8_t*)data, sizeof(uint16_t) * w * h);
}

void Ili9341::invert_colors(bool invert)
{
  const auto cs = make_chip_select();
  write_command(invert ? 0x21U : 0x20U);
}

}
