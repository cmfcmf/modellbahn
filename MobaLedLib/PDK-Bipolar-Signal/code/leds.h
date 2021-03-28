#pragma once

#define FADE_DURATION 300

#define setDuty(X) { \
  setDutyRed(X);     \
  setDutyGreen(X);   \
}

// The µC has an 11-bit PWM
// For each LED, we only use half of the available PWM

#define setDutyRed(X) {        \
  PWMG0DTL = (X & 0b111) << 5; \
  PWMG0DTH = (X >> 3);         \
}

#define setDutyGreen(X) {           \
  PWMG2DTL = (X & 0b111) << 5;      \
  PWMG2DTH = (X >> 3) | 0b10000000; \
}

static void leds_off()
{
  setDutyRed(0);
  setDutyGreen(0x3FF);
}

static void leds_red()
{
  setDuty(0x3FF);
}

static void leds_green()
{
  setDuty(0);
}

#define PWM_CLK_PRESCALER       2
#define PWM_COUNTER_UPPER_BOUND 0b1111111111 // 10 bit value, should not be changed

#define _PWM_FREQ 16000000 / (PWM_CLK_PRESCALER * (PWM_COUNTER_UPPER_BOUND + 1))

// https://www.mikrocontroller.net/articles/Pulsweitenmodulation#Mit_welcher_Frequenz_dimmt_man.3F
#if _PWM_FREQ < 1000
#error Calculated PWM frequency is lower than 1000kHz. Decrease the PWM_CLK_PRESCALER
#endif
#if _PWM_FREQ > 9000
#error Calculated PWM frequency is higher than 9000kHz. Increase the PWM_CLK_PRESCALER
#endif

#if PWM_COUNTER_UPPER_BOUND >= 0b10000000000
#error PWM_COUNTER_UPPER_BOUND is a 10 bit value
#endif

#if PWM_CLK_PRESCALER == 1
  #define _PWM_CLK_PRESCALER PWMGCLK_PRESCALE_NONE
#elif PWM_CLK_PRESCALER == 2
  #define _PWM_CLK_PRESCALER PWMGCLK_PRESCALE_DIV2
#elif PWM_CLK_PRESCALER == 4
  #define _PWM_CLK_PRESCALER PWMGCLK_PRESCALE_DIV4
#elif PWM_CLK_PRESCALER == 8
  #define _PWM_CLK_PRESCALER PWMGCLK_PRESCALE_DIV8
#elif PWM_CLK_PRESCALER == 6
  #define _PWM_CLK_PRESCALER PWMGCLK_PRESCALE_DIV16
#elif PWM_CLK_PRESCALER == 2
  #define _PWM_CLK_PRESCALER PWMGCLK_PRESCALE_DIV32
#elif PWM_CLK_PRESCALER == 4
  #define _PWM_CLK_PRESCALER PWMGCLK_PRESCALE_DIV64
#elif PWM_CLK_PRESCALER == 8
  #define _PWM_CLK_PRESCALER PWMGCLK_PRESCALE_DIV128
#else
#error Unsupported PWM_CLK_PRESCALER
#endif


static inline void leds_setup()
{
  PB &=~   (1 << 4) | (1 << 5); // Low
  PBC |=   (1 << 4) | (1 << 5); // Output
  PBPH &=~ (1 << 4) | (1 << 5); // no pull-up
  PBPL &=~ (1 << 4) | (1 << 5); // no pull-down

  PWMG0C = PWMG0C_OUT_PB4 | PWMG0C_OUT_PWMG0;
  PWMG2C = PWMG2C_OUT_PB5 | PWMG2C_OUT_PWMG2 | PWMG2C_INVERT_OUT;

  // TODO: Move this elsewhere, since it is also used by the heartbeat LED
  PWMGCUBL = (PWM_COUNTER_UPPER_BOUND & 0b11) << 6;
  PWMGCUBH = PWM_COUNTER_UPPER_BOUND >> 2;
  PWMGCLK = (uint8_t)(PWMGCLK_PWMG_ENABLE | _PWM_CLK_PRESCALER | PWMGCLK_CLK_IHRC);

  leds_off();
}

static void updateFromPwmValues() {
  setDutyRed(pwm_red);
  setDutyGreen(0x3FF - pwm_green);
}