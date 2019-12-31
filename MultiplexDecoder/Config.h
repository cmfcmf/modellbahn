#ifndef CONFIG_H_
#define CONFIG_H_ 1

#define DEMO 0       // 0: no demo, 1: demo (cycle aspects), 2: demo (random aspects)
#define USE_SERIAL 1 // 0: no debug output on Serial, 1: debug output on Serial

#define DARK_MAIN_SIGNAL_ASPECT    1 // 0: no support for darkening main signals, 1: enable darkening main signals
#define DARK_DISTANT_SIGNAL_ASPECT 1 // 0: no support for darkening distant signals, 1: enable darkening distant signals

constexpr int NUM_LEDS = 12;

// Time it takes for all 12 charlieplexed leds to cycle through -> if this was 12ms, each led would light up for 1ms.
constexpr uint32_t CHARLIE_CYCLE_LENGTH_US = 1200ULL;

// The brightness of each led can be controlled in DIMM_STEPS steps.
constexpr byte DIMM_STEPS = 20;

// Time it should take for an led to turn on/off.
constexpr auto DIMM_TIME_US = 200UL * 1000UL;



// CALCULATED SETTINGS

// Interval in which signal->dimm() must be called
constexpr auto DIMM_CALLING_INTERVAL_US = DIMM_TIME_US / DIMM_STEPS;
constexpr byte DIMM_UP_TOGGLE_DIMM_DIRECTION_IN = DIMM_TIME_US * 0.8 / DIMM_CALLING_INTERVAL_US;

/// Timer 1
constexpr auto TIMER1_PRESCALER = 256;
constexpr uint16_t OCR1A_VALUE = DIMM_CALLING_INTERVAL_US * 16000000ULL / 1000ULL / 1000ULL / TIMER1_PRESCALER;

/// Timer 2
constexpr auto TIMER2_PRESCALER = 64;
constexpr uint8_t OCR2A_VALUE = CHARLIE_CYCLE_LENGTH_US * 16000000ULL / 1000ULL / 1000ULL / TIMER2_PRESCALER / NUM_LEDS;



#endif
