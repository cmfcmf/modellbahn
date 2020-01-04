#ifndef DIMMING_H_
#define DIMMING_H_ 1

#include "Config.h"
#include "Signal.h"

// We cannot have these templates in the main .ino file, because the Arduino IDE
// generates wrong forward declarations for them.

extern Signal<PINS1> signal1;
extern Signal<PINS2> signal2;
extern Signal<PINS3> signal3;
extern Signal<PINS4> signal4;

template<typename S> static inline void doDimmInner(S & _signal, const uint8_t & ledIdx, const uint16_t & mask) {
  auto & led = _signal.m_leds[ledIdx];
  if (led.toggleDimmDirectionIn > 0) {
    --led.toggleDimmDirectionIn;
    if (led.toggleDimmDirectionIn == 0) {
      if (_signal.m_ledState & mask) { // bit was on
        _signal.m_ledState &= ~mask;
      } else {
        _signal.m_ledState |= mask;
      }
    }
  }

  if (_signal.m_ledState & mask) {
    if (led.dimmValue < DIMM_STEPS) {
      led.dimmValue += 1;
    }
  } else {
    if (led.dimmValue > 0) {
      led.dimmValue -= 1;
    }
  }
}

template<uint8_t ledIdx> static inline void doDimm1() {
  uint16_t mask = 1 << ledIdx;
  doDimmInner<Signal<PINS1>>(signal1, ledIdx, mask);
  doDimm1<ledIdx + 1>();
}
template<> inline void doDimm1<NUM_LEDS>() {}

template<uint8_t ledIdx> static inline void doDimm2() {
  uint16_t mask = 1 << ledIdx;
  doDimmInner<Signal<PINS2>>(signal2, ledIdx, mask);
  doDimm2<ledIdx + 1>();
}
template<> inline void doDimm2<NUM_LEDS>() {}

template<uint8_t ledIdx> static inline void doDimm3() {
  uint16_t mask = 1 << ledIdx;
  doDimmInner<Signal<PINS3>>(signal3, ledIdx, mask);
  doDimm3<ledIdx + 1>();
}
template<> inline void doDimm3<NUM_LEDS>() {}

template<uint8_t ledIdx> static inline void doDimm4() {
  uint16_t mask = 1 << ledIdx;
  doDimmInner<Signal<PINS4>>(signal4, ledIdx, mask);
  doDimm4<ledIdx + 1>();
}
template<> inline void doDimm4<NUM_LEDS>() {}

#endif
