#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <Arduino.h>
#include "Chaplex.h"
#include "AspectMapping.h"
#include "AbstractSignal.h"
#include "DigitalPin.h"

template <uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4>
class Signal : public AbstractSignal {
  public:
    // Call in ISR
    // Because everything is const or a template parameter, the switch statements are optimized and pre-calculated.
    __attribute__((always_inline)) __attribute__((optimize("O0"))) // TODO: Why O0?
    inline void charlieplex(const byte & cur_dimm, const byte & cur_led, const byte & high_led, const byte & low_led, const byte & lastHighLed, const byte & lastLowLed) {
      // ? instructions
      const bool outputState = m_leds[cur_led].dimmValue > cur_dimm;

      // 2 instructions
      switch (lastHighLed) {
        case 0:
          m_pin1.config(INPUT, false);
        break;
        case 1:
          m_pin2.config(INPUT, false);
        break;
        case 2:
          m_pin3.config(INPUT, false);
        break;
        default:
          m_pin4.config(INPUT, false);
        break;
      }
      // 1 instruction
      switch (lastLowLed) {
        case 0:
          m_pin1.mode(INPUT);
        break;
        case 1:
          m_pin2.mode(INPUT);
        break;
        case 2:
          m_pin3.mode(INPUT);
        break;
        default:
          m_pin4.mode(INPUT);
        break;
      }

      // In theory, we could skip the next two switch blocks entirely if outputState is false.
      // However, this would result in incosistent execution times of this method, which we're
      // trying to avoid.

      // 2 instructions
      switch (high_led) {
        case 0:
          m_pin1.config(OUTPUT, outputState);
        break;
        case 1:
          m_pin2.config(OUTPUT, outputState);
        break;
        case 2:
          m_pin3.config(OUTPUT, outputState);
        break;
        default:
          m_pin4.config(OUTPUT, outputState);
        break;
      }
      // 2 instructions
      switch (low_led) {
        case 0:
          m_pin1.config(OUTPUT, LOW);
        break;
        case 1:
          m_pin2.config(OUTPUT, LOW);
        break;
        case 2:
          m_pin3.config(OUTPUT, LOW);
        break;
        default:
          m_pin4.config(OUTPUT, LOW);
        break;
      }
    };
  protected:
    void detectMainSignal() override {
      Serial.print("| Main:    ");
      if (!detectLed(leds[RED])) {
        Serial.println("NO               |");
        return;
      }

      if (!detectLed(leds[YELLOW])) {
        Serial.println("YES - main       |");
        m_numAspects = sizeof(hv_main)/sizeof(hv_main[0]);
        m_mapping = const_cast<byte *>(&hv_main[0]);
        return;
      }
      if (!detectLed(leds[RED_RIGHT])) {
        Serial.println("YES - entry      |");
        m_numAspects = sizeof(hv_entry) / sizeof(hv_entry[0]);
        m_mapping = const_cast<byte *>(&hv_entry[0]);
        return;
      }

      Serial.println("YES - departure  |");
      m_numAspects = sizeof(hv_departure) / sizeof(hv_departure[0]);
      m_mapping = const_cast<byte *>(&hv_departure[0]);
    };

    void detectDistantSignal() override {
      Serial.print("| Distant: ");
      if (!detectLed(leds[VR_YELLOW_TOP])) {
        Serial.println("NO               |");
      } else {
        m_numDistantAspects = sizeof(hv_distant) / sizeof(hv_distant[0]);
        if (m_mapping) {
          Serial.println("YES - attached   |");
          m_distantMapping = const_cast<byte *>(&hv_distant[0]);
        } else {
          Serial.println("YES - standalone |");
          m_mapping = const_cast<byte *>(&hv_distant[0]);
        }
      }
    };
  private:
    DigitalPin<pin1> m_pin1;
    DigitalPin<pin2> m_pin2;
    DigitalPin<pin3> m_pin3;
    DigitalPin<pin4> m_pin4;

    inline bool detectLed(const charlieLed led) __attribute__((always_inline)) {
      m_pin1.config(INPUT, false); // All input without pullup
      m_pin2.config(INPUT, false);
      m_pin3.config(INPUT, false);
      m_pin4.config(INPUT, false);

      switch (led.c) {
        case 0:
          m_pin1.config(OUTPUT, LOW);
          break;
        case 1:
          m_pin2.config(OUTPUT, LOW);
          break;
        case 2:
          m_pin3.config(OUTPUT, LOW);
          break;
        default:
          m_pin4.config(OUTPUT, LOW);
          break;
      }

      delay(1);

      bool result;
      switch (led.r) {
        case 0:
          m_pin1.config(INPUT, true);
          delay(1);
          result = !m_pin1.read();
          break;
        case 1:
          m_pin2.config(INPUT, true);
          delay(1);
          result = !m_pin2.read();
          break;
        case 2:
          m_pin3.config(INPUT, true);
          delay(1);
          result = !m_pin3.read();
          break;
        default:
          m_pin4.config(INPUT, true);
          delay(1);
          result = !m_pin4.read();
          break;
      }

      m_pin1.config(INPUT, false); // All input without pullup
      m_pin2.config(INPUT, false);
      m_pin3.config(INPUT, false);
      m_pin4.config(INPUT, false);

      return result;
    };
};
#endif
