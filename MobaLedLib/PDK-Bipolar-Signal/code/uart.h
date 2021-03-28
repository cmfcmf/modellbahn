#include <util/delay.h>
#include <pdk/device.h>

volatile uint16_t txdata;

inline void send_bit()
{
  if (txdata) {
    if (txdata & 0b1) {
      PA |=  (1 << 7);
    } else {
      PA &=~ (1 << 7);
    }
    txdata >>= 1;
  }
}

#define INTERVAL 16*6 // serial port speed (9600)

static void uart0_send(char c)
{
  txdata = (c << 1) | 0x200;
  for (uint8_t i = 0; i < 11; i++)
  {
    send_bit();
    _delay_us(INTERVAL);
  }
}

static void uart0_setup(void) {
    PA |= (1 << 7);
    PAC |= (1 << 7);
}

void uart0_send_uint8_t(uint8_t val)  __reentrant
{
  uint8_t d = val / (uint8_t)100;
  uart0_send('0' + (char)d);
  val = val % (uint8_t)100;

  d = val / (uint8_t)10;
  uart0_send('0' + (char)d);
  val = val % (uint8_t)10;

  d = val;
  uart0_send('0' + (char)d);
}

void uart0_send_uint16_t(uint16_t val)  __reentrant
{
  uint8_t d = val / (uint16_t)10000;
  uart0_send('0' + (char)d);
  val = val % (uint16_t)10000;

  d = val / (uint16_t)1000;
  uart0_send('0' + (char)d);
  val = val % (uint16_t)1000;

  d = val / (uint8_t)100;
  uart0_send('0' + (char)d);
  val = val % (uint8_t)100;

  d = val / (uint8_t)10;
  uart0_send('0' + (char)d);
  val = val % (uint8_t)10;

  d = val;
  uart0_send('0' + (char)d);
}

void uart0_send_string(char *s) {
  while (*s) {
    uart0_send(*s++);
  }
}