#pragma once

#include <stdint.h>
#include <pdk/device.h>

static void measure_pwm_setup(void)
{
  PBC &=~   (1 << 1) | (1 << 2); // Input
  PBPH |=   (1 << 1) | (1 << 2); // Pull-up
  PBDIER |= (1 << 1) | (1 << 2); // Enable digital I/O

  T16M = (uint8_t)(T16M_CLK_IHRC | T16M_CLK_DIV1 | T16M_INTSRC_15BIT);
  T16C = 0;

  INTEGS |= INTEGS_T16_FALLING;
}

static volatile bool timeout = false;
static volatile char current_color = 'r';

static uint16_t _measure_pwm_red(void)
{
  current_color = 'r';
  T16C = 0;
  INTRQ &=~ INTRQ_T16;
  INTEN |= INTEN_T16;

  __asm__(
    "clear	p\n"
    // wait for high
    "1$:\n"
    "t1sn	 __pb, #1\n"
    "goto	1$\n"
    // wait for low
    "2$:\n"
    "t0sn	 __pb, #1\n"
    "goto	2$\n"
    // reset timer to 0
    "stt16	p\n"

    // wait for high
    "3$:\n"
    "t1sn	 __pb, #1\n"
    "goto	3$\n"

    "ldt16 _duty_red\n"

    // wait for low
    "4$:\n"
    "t0sn	 __pb, #1\n"
    "goto	4$\n"

    "ldt16 _period_red\n"
  );

  INTEN &=~ INTEN_T16;

  uint16_t pwm = 0x3FFUL * duty_red / period_red;

  timeout = false;
  __asm__("ABORT_LABEL_RED_IS_HERE$:");

  return pwm;
}

static uint16_t _measure_pwm_green(void)
{
  current_color = 'g';
  T16C = 0;
  INTRQ &=~ INTRQ_T16;
  INTEN |= INTEN_T16;

  __asm__(
    "clear	p\n"
    // wait for high
    "1$:\n"
    "t1sn	 __pb, #2\n"
    "goto	1$\n"
    // wait for low
    "2$:\n"
    "t0sn	 __pb, #2\n"
    "goto	2$\n"
    // reset timer to 0
    "stt16	p\n"

    // wait for high
    "3$:\n"
    "t1sn	 __pb, #2\n"
    "goto	3$\n"

    "ldt16 _duty_green\n"

    // wait for low
    "4$:\n"
    "t0sn	 __pb, #2\n"
    "goto	4$\n"

    "ldt16 _period_green\n"
  );

  INTEN &=~ INTEN_T16;

  uint16_t pwm = 0x3FFUL * duty_green / period_green;

  timeout = false;
  __asm__("ABORT_LABEL_GREEN_IS_HERE$:");

  return pwm;
}

static void measure_pwm(void)
{
  pwm_red = _measure_pwm_red();
  if (timeout) {
    pwm_red = (PB & (1 << 1)) ? 0 : 0x3FF;

#ifdef DEBUG
    // uart0_send('e');
    // uart0_send('r');
    // uart0_send('?');
    // _delay_ms(1000);
#endif
  }

  pwm_green = _measure_pwm_green();
  if (timeout) {
    pwm_green = (PB & (1 << 2)) ? 0 : 0x3FF;

#ifdef DEBUG
    // uart0_send('e');
    // uart0_send('g');
    // uart0_send('?');
    // _delay_ms(1000);
#endif
  }
}

static inline void measure_pwm_irq_handler()
{
  INTEN &=~ INTEN_T16;
  timeout = true;

  if (current_color == 'r') {
    __asm__(
      "mov a, sp\n"
      "add a, #-6\n"
      "mov p, a\n"
      "mov a, #0x"
      "000186" // REPLACE_ADDRESS_RED_MARKER
      "/2\n"
      "idxm p, a\n"

      "mov a, sp\n"
      "add a, #-5\n"
      "mov p, a\n"
      "mov a, #0x"
      "000186" // REPLACE_ADDRESS_RED_MARKER
      "/2/256\n"
      "idxm p, a\n"
    );
  } else {
    __asm__(
      "mov a, sp\n"
      "add a, #-6\n"
      "mov p, a\n"
      "mov a, #0x"
      "00020A" // REPLACE_ADDRESS_GREEN_MARKER
      "/2\n"
      "idxm p, a\n"

      "mov a, sp\n"
      "add a, #-5\n"
      "mov p, a\n"
      "mov a, #0x"
      "00020A" // REPLACE_ADDRESS_GREEN_MARKER
      "/2/256\n"
      "idxm p, a\n"
    );
  }
}