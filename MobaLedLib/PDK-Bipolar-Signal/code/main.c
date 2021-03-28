/*
 * https://free-pdk.github.io/chips/PFS173
 * http://www.padauk.com.tw/upload/doc/PFS173%20datasheet_v105_EN_20200619.pdf
 *
 * https://free-pdk.github.io/tutorial
 * http://sdcc.sourceforge.net/doc/sdccman.pdf
*/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

// must be at the very top to make sure that these are word-aligned
uint16_t duty_red;
uint16_t period_red;
uint16_t duty_green;
uint16_t period_green;

// these do not have to be word-aligned
uint16_t pwm_red;
uint16_t pwm_green;

#include <pdk/device.h>
#include <easy-pdk/serial_num.h>
#include <util/auto_sysclock.h>

// #define DEBUG

#include "main.h"
#ifdef DEBUG
#include "uart.h"
#endif
#include "leds.h"
#include "measure-pwm.h"
#include "heartbeat.h"


// B1: PWM Signal 1
// B2: PWM Signal 2
// B4: LED 1
// B5: LED 2

// A0: (?)
// A3: (Prog)
// A4: Heartbeat
// A5: (Prog)
// A6: (Prog)
// A7: Serial TX

void interrupt(void) __interrupt(0) {
  if (INTRQ & INTRQ_T16) {
    INTRQ &=~ INTRQ_T16;
    if (INTEN & INTEN_T16) {
      measure_pwm_irq_handler();
    }
  }
}

inline void setup(void)
{
  INTEN = 0;
  MISC = (uint8_t)(MISC_FAST_WAKEUP_DISABLE | MISC_LCD_DISABLE | MISC_LVR_ENABLE | MISC_WATCHDOG_256K_ILRC);

  heartbeat_setup();

  leds_setup();

#ifdef DEBUG
  uart0_setup();
  // For auto baud detection by the free-pdk programmer
  uart0_send(0x55);
#endif

  measure_pwm_setup();

  INTRQ = 0;
  __engint();

  __wdreset();
  CLKMD |= CLKMD_ENABLE_WATCHDOG;
}

void main(void)
{
  setup();

  while (true) {
    measure_pwm();
    __wdreset();

#ifdef DEBUG
    uart0_send('r');
    uart0_send(' ');
    uart0_send_uint8_t(pwm_red >> 2);
    uart0_send(' ');
    uart0_send_uint8_t(pwm_red & 0b11);
    uart0_send(' ');
    uart0_send('g');
    uart0_send(' ');
    uart0_send_uint8_t(pwm_green >> 2);
    uart0_send(' ');
    uart0_send_uint8_t(pwm_green & 0b11);
    uart0_send('\n');
#endif

    updateFromPwmValues();
    __wdreset();

    heartbeat_beat();
    __wdreset();
  }
}

unsigned char _sdcc_external_startup(void)
{
  __wdreset();
  __disgint();
  CLKMD &=~ CLKMD_ENABLE_WATCHDOG;
  MISCLVR = (uint8_t)MISCLVR_3V5;

  PDK_SET_FUSE(FUSE_SECURITY_OFF | FUSE_PB4_PB5_STRONG | FUSE_BOOTUP_SLOW);

  AUTO_INIT_SYSCLOCK();
  AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

  ROP = ROP_TMX_16MHZ | ROP_PWM_16MHZ;

  return 0;
}