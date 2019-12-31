#ifndef ABSTRACT_SIGNAL_H_
#define ABSTRACT_SIGNAL_H_ 1

#include <Arduino.h>
#include "Config.h"
#include "CharlieLed.h"
#include "AspectMapping.h"

#pragma GCC optimize ("O3")
#pragma GCC optimize ("unroll-loops")

class AbstractSignal {
  friend class SignalBuilder;
  public:
    AbstractSignal();

    // Call right after construction
    AbstractSignal* begin();

    // Call when programming
    virtual void clearSavedState();

    // Change current aspect
    void setAspect(byte, bool = true);
    void setDistantAspect(byte, bool = true);

    __attribute__((always_inline))
    inline void dimm(const byte & ledIdx, const uint16_t & mask) {
      auto & led = m_leds[ledIdx];
      if (led.toggleDimmDirectionIn != 0) {
        if (--led.toggleDimmDirectionIn == 0) {
          if (m_ledState & mask) { // bit was on
            m_ledState &= ~mask;
          } else {
            m_ledState |= mask;
          }
        }
      }

      if (m_ledState & mask) {
        if (led.dimmValue < DIMM_STEPS) {
          led.dimmValue += 1;
        }
      } else {
        if (led.dimmValue > 0) {
          led.dimmValue -= 1;
        }
      }
    };

    // Signal configuration checks
    bool isSignalConnected(void);
    bool isAdditionalDistantSignalConnected(void);

    #if DEMO == 2
    void setRandomAspect(void);
    #endif

    static constexpr CharlieLed leds[NUM_LEDS] = {
      { 1 , 0 }, // 00
      { 1 , 2 }, // 01 (VR)
      { 1 , 3 }, // 02
      { 0 , 1 }, // 03
      { 2 , 1 }, // 04 (VR)
      { 3 , 1 }, // 05
      { 0 , 2 }, // 06 (VR)
      { 0 , 3 }, // 07
      { 2 , 0 }, // 08 (VR)
      { 3 , 0 }, // 09
      { 2 , 3 }, // 10
      { 3 , 2 }  // 11
    };

    enum SignalAspects {
      HP0 = 0,
      HP1 = 1,
      HP2 = 2,
      SH1 = 3,
      HP00 = 4,
      VR0 = 5,
      VR1 = 6,
      VR2 = 7,
      DARK = 8,
      VR_DARK = 9,
    };

    enum SIGNAL_LED_MAPPINGS {
      RED = 2,
      GREEN = 5,
      YELLOW = 3,
      WHITE_TOP = 7,
      WHITE_BOTTOM = 9,
      RED_RIGHT = 0,

      VR_YELLOW_TOP = 1,
      VR_GREEN_TOP = 4,
      VR_YELLOW_BOTTOM = 6,
      VR_GREEN_BOTTOM = 8,
    };
  protected:
    void detect();
    virtual void detectMainSignal() = 0;
    virtual void detectDistantSignal() = 0;

    void saveState();
    void applySavedState();

    void applyAspectChange(AspectMapping);

    byte m_id;
    static byte nextId;

    byte m_numAspects = 0;
    byte *m_mapping = NULL;
    byte m_numDistantAspects = 0;
    byte *m_distantMapping = NULL;

    AspectMapping m_currentAspect;
    AspectMapping m_currentDistantAspect;
    byte m_currentAspectIndex;
    byte m_currentDistantAspectIndex;

    static constexpr uint16_t m_main_mask = (1L << RED) | (1L << GREEN) | (1L << YELLOW) | (1L << WHITE_TOP) | (1L << WHITE_BOTTOM) | (1L << RED_RIGHT);
    static constexpr uint16_t m_distant_mask = (1L << VR_YELLOW_TOP) | (1L << VR_YELLOW_BOTTOM) | (1L << VR_GREEN_TOP) | (1L << VR_GREEN_BOTTOM);

    const AspectMapping m_aspects[10] = {
      {HP0,  (1L << RED),                                              m_main_mask, true},
      {HP1,  (1L << GREEN),                                            m_main_mask, false},
      {HP2,  (1L << GREEN) | (1L << YELLOW),                           m_main_mask, false},
      {SH1,  (1L << RED)   | (1L << WHITE_TOP) | (1L << WHITE_BOTTOM), m_main_mask, true},
      {HP00, (1L << RED)   | (1L << RED_RIGHT),                        m_main_mask, true},

      {VR0,  (1L << VR_YELLOW_TOP)    | (1L << VR_YELLOW_BOTTOM), m_distant_mask, false},
      {VR1,  (1L << VR_GREEN_TOP)     | (1L << VR_GREEN_BOTTOM),  m_distant_mask, false},
      {VR2,  (1L << VR_YELLOW_BOTTOM) | (1L << VR_GREEN_TOP),     m_distant_mask, false},

#if DARK_MAIN_SIGNAL_ASPECT == 1
      {DARK,    0, m_main_mask, true},
#endif
#if DARK_DISTANT_SIGNAL_ASPECT == 1
      {VR_DARK, 0, m_distant_mask, false}
#endif
    };

#if DARK_MAIN_SIGNAL_ASPECT == 1
    const byte hv_main[3]      = {HP0,  HP1,           DARK};
    const byte hv_entry[4]     = {HP0,  HP1, HP2,      DARK};
    const byte hv_departure[5] = {HP00, HP1, HP2, SH1, DARK};
#else
    const byte hv_main[2]      = {HP0,  HP1,         };
    const byte hv_entry[3]     = {HP0,  HP1, HP2,    };
    const byte hv_departure[4] = {HP00, HP1, HP2, SH1};
#endif
#if DARK_DISTANT_SIGNAL_ASPECT == 1
    const byte hv_distant[4]   = {VR0,  VR1, VR2,      VR_DARK};
#else
    const byte hv_distant[3]   = {VR0,  VR1, VR2};
#endif
    // Bitmask for the 12 LEDs indicating which should currently be on and which should be off.
    uint16_t m_ledState = 0x0000;
    struct {
      byte toggleDimmDirectionIn;     // Zeit bis zum nächsten Wechsel
      byte dimmValue; // current dimm value
    } m_leds[NUM_LEDS];
};
#endif
