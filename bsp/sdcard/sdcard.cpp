#include "bsp/sdcard/sdcard.h"
#include "bsp/spi/chip_select.h"
#include <cstdint>

namespace mart {

constexpr uint8_t DATA_TOKEN_CMD9 = 0xFEU;
constexpr uint8_t DATA_TOKEN_CMD17 = 0xFEU;
constexpr uint8_t DATA_TOKEN_CMD18 = 0xFEU;
constexpr uint8_t DATA_TOKEN_CMD24 = 0xFEU;
constexpr uint8_t DATA_TOKEN_CMD25 = 0xFCU;


int Sdcard::init(Pin clk, Pin mosi, Pin miso, uint32_t spi_alternate)
{
  clk.init(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, spi_alternate);
  miso.init(GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, spi_alternate);
  mosi.init(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, spi_alternate);

  SPI_InitTypeDef config;
  config.Mode = SPI_MODE_MASTER;
  config.Direction = SPI_DIRECTION_2LINES;
  config.DataSize = SPI_DATASIZE_8BIT;
  config.CLKPolarity = SPI_POLARITY_LOW;
  config.CLKPhase = SPI_PHASE_1EDGE;
  config.NSS = SPI_NSS_SOFT;
  config.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  config.FirstBit = SPI_FIRSTBIT_MSB;
  config.TIMode = SPI_TIMODE_DISABLE;
  config.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  config.CRCPolynomial = 10;
  config.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  config.NSSPMode = SPI_NSS_PULSE_DISABLE;

  if (SpiDevice::init(config) != HAL_OK)
  {
    return -1;
  }

  /*
    Step 1.

    Set DI and CS high and apply 74 or more clock pulses to SCLK. Without this
    step under certain circumstances SD-card will not work. For instance, when
    multiple SPI devices are sharing the same bus (i.e. MISO, MOSI, CS).
  */

  for (int i = 0; i < 10; i++) // 80 clock pulses
  {
    if (transmit(0xFF) != HAL_OK)
    {
      return -2;
    }
  }

  const auto cs = make_chip_select();

  /*
    Step 2.

    Send CMD0 (GO_IDLE_STATE): Reset the SD card.
  */
  if (wait_not_busy() < 0)
  {
    return -3;
  }

  {
    static const uint8_t cmd[] =
      { 0x40 | 0x00 /* CMD0 */, 0x00, 0x00, 0x00, 0x00 /* ARG = 0 */, (0x4A << 1) | 1 /* CRC7 + end bit */ };
    transmit((uint8_t*)cmd, sizeof(cmd));
  }

  if (read_r1() != 0x01)
  {
    return -4;
  }

  /*
    Step 3.

    After the card enters idle state with a CMD0, send a CMD8 with argument of
    0x000001AA and correct CRC prior to initialization process. If the CMD8 is
    rejected with illigal command error (0x05), the card is SDC version 1 or
    MMC version 3. If accepted, R7 response (R1(0x01) + 32-bit return value)
    will be returned. The lower 12 bits in the return value 0x1AA means that
    the card is SDC version 2 and it can work at voltage range of 2.7 to 3.6
    volts. If not the case, the card should be rejected.
  */
  if (wait_not_busy() < 0)
  {
    return -5;
  }

  {
    static const uint8_t cmd[] =
      { 0x40 | 0x08 /* CMD8 */, 0x00, 0x00, 0x01, 0xAA /* ARG */, (0x43 << 1) | 1 /* CRC7 + end bit */ };
    transmit((uint8_t*)cmd, sizeof(cmd));
  }

  if (read_r1() != 0x01)
  {
    return -6; // not an SDHC/SDXC card (i.e. SDSC, not supported)
  }

  {
    uint8_t resp[4];
    if (read_bytes(resp, sizeof(resp)) < 0)
    {
      return -7;
    }

    if (((resp[2] & 0x01) != 1) || (resp[3] != 0xAA))
    {
      return -8;
    }
  }

  /*
    Step 4.

    And then initiate initialization with ACMD41 with HCS flag (bit 30).
  */
  for (;;) {
    if (wait_not_busy() < 0)
    {
      return -9;
    }

    {
      static const uint8_t cmd[] =
        { 0x40 | 0x37 /* CMD55 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
      transmit((uint8_t*)cmd, sizeof(cmd));
    }

    if (read_r1() != 0x01)
    {
      return -10;
    }

    if (wait_not_busy() < 0)
    {
      return -11;
    }

    {
      static const uint8_t cmd[] =
        { 0x40 | 0x29 /* ACMD41 */, 0x40, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
      transmit((uint8_t*)cmd, sizeof(cmd));
    }

    const uint8_t r1 = read_r1();
    if (r1 == 0x00)
    {
      break;
    }

    if(r1 != 0x01)
    {
      return -12;
    }
  }

  /*
    Step 5.

    After the initialization completed, read OCR register with CMD58 and check
    CCS flag (bit 30). When it is set, the card is a high-capacity card known
    as SDHC/SDXC.
  */
  if (wait_not_busy() < 0)
  {
    return -13;
  }

  {
    static const uint8_t cmd[] =
      { 0x40 | 0x3A /* CMD58 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
    transmit((uint8_t*)cmd, sizeof(cmd));
  }

  if (read_r1() != 0x00)
  {
    return -14;
  }

  {
    uint8_t resp[4];
    if (read_bytes(resp, sizeof(resp)) < 0)
    {
      return -15;
    }

    if ((resp[0] & 0xC0) != 0xC0)
    {
      return -16;
    }
  }

    return 0;
}

int Sdcard::get_blocks_number(uint32_t* number)
{
  uint8_t csd[16];
  uint8_t crc[2];

  {
    const auto cs = make_chip_select();

    if (wait_not_busy() < 0)
    {
      return -1;
    }

    /* CMD9 (SEND_CSD) command */
    {
      static const uint8_t cmd[] =
        { 0x40 | 0x09 /* CMD9 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 /* CRC7 + end bit */ };
      transmit((uint8_t*)cmd, sizeof(cmd));
    }

    if (read_r1() != 0x00)
    {
      return -2;
    }

    if (wait_data_token(DATA_TOKEN_CMD9) < 0)
    {
      return -3;
    }

    if (read_bytes(csd, sizeof(csd)) < 0) {
      return -4;
    }

    if (read_bytes(crc, sizeof(crc)) < 0) {
      return -5;
    }
  }

  // first byte is VVxxxxxxxx where VV is csd.version
   if((csd[0] & 0xC0) != 0x40) // csd.version != 1
        return -6;

   uint32_t tmp = csd[7] & 0x3F; // two bits are reserved
   tmp = (tmp << 8) | csd[8];
   tmp = (tmp << 8) | csd[9];
   // Full volume: (C_SIZE+1)*512KByte == (C_SIZE+1)<<19
   // Block size: 512Byte == 1<<9
   // Blocks number: CARD_SIZE/BLOCK_SIZE = (C_SIZE+1)*(1<<19) / (1<<9) = (C_SIZE+1)*(1<<10)
   tmp = (tmp + 1) << 10;
   *number = tmp;

   return 0;
}


int Sdcard::read_single_block(uint32_t block_num, uint8_t* data)
{
  uint8_t crc[2];

  const auto cs = make_chip_select();

  if (wait_not_busy() < 0)
  {
    return -1;
  }

  /* CMD17 (SEND_SINGLE_BLOCK) command */
  uint8_t cmd[] = {
    0x40 | 0x11 /* CMD17 */,
    static_cast<uint8_t>((block_num >> 24) & 0xFFU), /* ARG */
    static_cast<uint8_t>((block_num >> 16) & 0xFFU),
    static_cast<uint8_t>((block_num >> 8) & 0xFFU),
    static_cast<uint8_t>(block_num & 0xFFU),
    (0x7F << 1) | 1 /* CRC7 + end bit */
  };
  transmit((uint8_t*)cmd, sizeof(cmd));

  if (read_r1() != 0x00)
  {
    return -2;
  }

  if (wait_data_token(DATA_TOKEN_CMD17) < 0)
  {
    return -3;
  }

  if (read_bytes(data, BLOCK_SIZE) < 0) {
    return -4;
  }

  if (read_bytes(crc, sizeof(crc)) < 0) {
    return -5;
  }

  return 0;
}

int Sdcard::write_single_block(uint32_t block_num, const uint8_t *data)
{
  const auto cs = make_chip_select();

  if (wait_not_busy() < 0)
  {
    return -1;
  }

  /* CMD24 (WRITE_BLOCK) command */
  uint8_t cmd[] = {
    0x40 | 0x18 /* CMD24 */,
    static_cast<uint8_t>((block_num >> 24) & 0xFFU), /* ARG */
    static_cast<uint8_t>((block_num >> 16) & 0xFFU),
    static_cast<uint8_t>((block_num >> 8) & 0xFFU),
    static_cast<uint8_t>(block_num & 0xFFU),
    (0x7F << 1) | 1 /* CRC7 + end bit */
  };
  transmit((uint8_t*)cmd, sizeof(cmd));

  if (read_r1() != 0x00)
  {
    return -2;
  }

  uint8_t dataToken = DATA_TOKEN_CMD24;
  uint8_t crc[2] = { 0xFF, 0xFF };
  transmit(&dataToken, sizeof(dataToken));
  transmit((uint8_t*)data, BLOCK_SIZE);
  transmit(crc, sizeof(crc));

  /*
    dataResp:
    xxx0abc1
    010 - Data accepted
    101 - Data rejected due to CRC error
    110 - Data rejected due to write error
  */
  uint8_t dataResp;
  read_bytes(&dataResp, sizeof(dataResp));
  if ((dataResp & 0x1F) != 0x05) // data rejected
  {
    return -3;
  }

  if (wait_not_busy() < 0)
  {
    return -4;
  }

  return 0;
}

int Sdcard::read_begin(uint32_t block_num)
{
  const auto cs = make_chip_select();

  if (wait_not_busy() < 0)
  {
    return -1;
  }

  /* CMD18 (READ_MULTIPLE_BLOCK) command */
  uint8_t cmd[] = {
    0x40 | 0x12 /* CMD18 */,
    static_cast<uint8_t>((block_num >> 24) & 0xFFU), /* ARG */
    static_cast<uint8_t>((block_num >> 16) & 0xFFU),
    static_cast<uint8_t>((block_num >> 8) & 0xFFU),
    static_cast<uint8_t>(block_num & 0xFFU),
    (0x7F << 1) | 1 /* CRC7 + end bit */
  };
  transmit((uint8_t*)cmd, sizeof(cmd));

  if (read_r1() != 0x00)
  {
    return -2;
  }

  return 0;
}

int Sdcard::read_data(uint8_t* data)
{
  uint8_t crc[2];

  const auto cs = make_chip_select();

  if(wait_data_token(DATA_TOKEN_CMD18) < 0)
  {
    return -1;
  }

  if(read_bytes(data, BLOCK_SIZE) < 0)
  {
    return -2;
  }

  if(read_bytes(crc, 2) < 0)
  {
    return -3;
  }

  return 0;
}

int Sdcard::read_end()
{
  const auto cs = make_chip_select();

  /* CMD12 (STOP_TRANSMISSION) */
  {
    static const uint8_t cmd[] = { 0x40 | 0x0C /* CMD12 */, 0x00, 0x00, 0x00, 0x00 /* ARG */, (0x7F << 1) | 1 };
    transmit((uint8_t*)cmd, sizeof(cmd));
  }

  /*
    The received byte immediataly following CMD12 is a stuff byte, it should be
    discarded before receive the response of the CMD12
  */
  uint8_t stuff_byte;
  if (read_bytes(&stuff_byte, sizeof(stuff_byte)) < 0) {
    return -1;
  }

  if (read_r1() != 0x00)
  {
    return -2;
  }

  return 0;
}

int Sdcard::write_begin(uint32_t block_num)
{
  const auto cs = make_chip_select();

  if (wait_not_busy() < 0)
  {
    return -1;
  }

  /* CMD25 (WRITE_MULTIPLE_BLOCK) command */
  uint8_t cmd[] = {
    0x40 | 0x19 /* CMD25 */,
    static_cast<uint8_t>((block_num >> 24) & 0xFFU), /* ARG */
    static_cast<uint8_t>((block_num >> 16) & 0xFFU),
    static_cast<uint8_t>((block_num >> 8) & 0xFFU),
    static_cast<uint8_t>(block_num & 0xFFU),
    (0x7F << 1) | 1 /* CRC7 + end bit */
  };
  transmit((uint8_t*)cmd, sizeof(cmd));

  if (read_r1() != 0x00)
  {
    return -2;
  }

  return 0;
}

int Sdcard::write_data(const uint8_t* data)
{
  const auto cs = make_chip_select();

  transmit(DATA_TOKEN_CMD25);
  transmit((uint8_t*)data, BLOCK_SIZE);
  uint8_t crc[2] = { 0xFF, 0xFF };
  transmit(crc, sizeof(crc));

  /*
    dataResp:
    xxx0abc1
    010 - Data accepted
    101 - Data rejected due to CRC error
    110 - Data rejected due to write error
  */
  uint8_t data_resp;
  read_bytes(&data_resp, sizeof(data_resp));
  if ((data_resp & 0x1F) != 0x05) // data rejected
  {
    return -1;
  }

  if (wait_not_busy() < 0)
  {
    return -2;
  }

  return 0;
}

int Sdcard::write_end()
{
  const auto cs = make_chip_select();

  const uint8_t stop_transition_for_cmd25 = 0xFDU;
  transmit(stop_transition_for_cmd25);

  // skip one byte before readyng "busy"
  // this is required by the spec and is necessary for some real SD-cards!
  uint8_t skip_byte;
  read_bytes(&skip_byte, sizeof(skip_byte));

  if (wait_not_busy() < 0)
  {
    return -1;
  }

  return 0;
}

/*
R1: 0abcdefg
     ||||||`- 1th bit (g): card is in idle state
     |||||`-- 2th bit (f): erase sequence cleared
     ||||`--- 3th bit (e): illigal command detected
     |||`---- 4th bit (d): crc check error
     ||`----- 5th bit (c): error in the sequence of erase commands
     |`------ 6th bit (b): misaligned addres used in command
     `------- 7th bit (a): command argument outside allowed range
             (8th bit is always zero)
*/
int Sdcard::read_r1()
{
  uint8_t r1;
  // make sure FF is transmitted during receive
  uint8_t tx = 0xFF;
  for(;;) {
    HAL_SPI_TransmitReceive(&hspi_, &tx, &r1, sizeof(r1), HAL_MAX_DELAY);
    if((r1 & 0x80) == 0) // 8th bit alwyas zero, r1 recevied
      break;
  }
  return r1;
}

int Sdcard::read_bytes(uint8_t* data, size_t size)
{
  // make sure FF is transmitted during receive
  uint8_t tx = 0xFF;
  while(size > 0) {
    HAL_SPI_TransmitReceive(&hspi_, &tx, data, 1, HAL_MAX_DELAY);
    data++;
    size--;
  }

  return 0;
}

int Sdcard::wait_not_busy()
{
  uint8_t busy;
  do {
    if(read_bytes(&busy, sizeof(busy)) < 0) {
      return -1;
    }
  } while(busy != 0xFF);

  return 0;
}

int Sdcard::wait_data_token(const uint8_t token)
{
  uint8_t fb;
  // make sure FF is transmitted during receive
  uint8_t tx = 0xFF;
  for(;;) {
    HAL_SPI_TransmitReceive(&hspi_, &tx, &fb, sizeof(fb), HAL_MAX_DELAY);
    if(fb == token)
    {
      break;
    }

    if(fb != 0xFF)
    {
      return -1;
    }
  }
  return 0;
}

}  // mart
