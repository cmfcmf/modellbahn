#pragma once

#include <pdk/device.h>

static void heartbeat_setup(void)
{
  // Heartbeat LED
  PAC  |=  (1 << 4); // Output
  PA   |=  (1 << 4); // High
  PAPH &=~ (1 << 4); // no pull-up
  // PAPL &=~ (1 << 4); // no pull-down
  PWMG1C = PWMG1C_OUT_PA4 | PWMG1C_OUT_PWMG1 | PWMG1C_INVERT_OUT;

  // Timer is incremented roughly every 1ms
  TM2C = TM2C_CLK_ILRC | TM2C_OUT_DISABLE;
  TM2S = TM2S_PRESCALE_DIV16 | TM2S_SCALE_DIV6;
  TM2B = 0xFF;
  TM2CT = 0;
  INTEN &=~ INTEN_TM2; // No interrupt
}

// We don't _really_ need this table for the hearbeat LED, but why not.
#define PWM_TABLE_ENTRIES 64
_CODE const uint16_t pwmtable_11[PWM_TABLE_ENTRIES] =
{
     0,    0,    0,    1,    1,    1,    1,    2,
     2,    2,    3,    3,    4,    4,    5,    6,
     7,    8,    9,   10,   11,   13,   14,   16,
    19,   21,   24,   27,   31,   35,   39,   44,
    50,   56,   64,   72,   81,   91,  103,  116,
   131,  148,  167,  188,  212,  239,  269,  303,
   342,  385,  434,  489,  551,  621,  700,  789,
   889, 1001, 1128, 1271, 1432, 1613, 1817, 2047
};

static void heartbeat_beat(void)
{
  static uint8_t lastTimerValue = 0;
  uint8_t diff = TM2CT - lastTimerValue;

  if (diff >= 16) {
    diff /= (uint8_t)16;
    static uint8_t val = 0;
    while (diff--) {
      static bool dir = true;
      if (dir) {
        val++;
        if (val == PWM_TABLE_ENTRIES - 1) {
          dir = false;
        }
      } else {
        val--;
        if (val == 0) {
          dir = true;
        }
      }
    }

    const uint16_t pwm = pwmtable_11[val];
    PWMG1DTL = (pwm & 0b111) << 5;
    PWMG1DTH = (pwm & 0b11111111000) >> 3;
    lastTimerValue = TM2CT;
  }
}