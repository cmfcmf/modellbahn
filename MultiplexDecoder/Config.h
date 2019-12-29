#ifndef CONFIG_H_
#define CONFIG_H_ 1

#define DEMO 1 // 0: no demo, 1: demo (cycle aspects), 2: demo (random aspects)

constexpr int NUM_LEDS = 12;

// Time it takes for all 12 charlieplexed leds to cycle through -> if this was 12ms, each led would light up for 1ms.
constexpr uint32_t CHARLIE_CYCLE_LENGTH_US = 1200ULL;

// The brightness of each led can be controlled in DIMM_STEPS steps.
constexpr byte DIMM_STEPS = 20;

// Time it should take for a led to turn on/off.
constexpr auto DIMM_TIME_US = 200UL * 1000UL;



// CALCULATED SETTINGS

constexpr auto PRESCALER = 64;
constexpr uint16_t OCR1A_VALUE = CHARLIE_CYCLE_LENGTH_US * 16000000ULL / 1000ULL / 1000ULL / PRESCALER / NUM_LEDS;

// Interval in which signal->dimm() is called
constexpr auto DIMM_CALLING_INTERVAL_US = DIMM_TIME_US / DIMM_STEPS;

constexpr byte DIMM_UP_TOGGLE_DIMM_DIRECTION_IN = DIMM_TIME_US * 0.8 / DIMM_CALLING_INTERVAL_US;

#endif
