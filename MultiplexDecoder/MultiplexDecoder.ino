#pragma GCC optimize ("O3")
#pragma GCC optimize ("unroll-loops")

#include "avr/wdt.h"

#include <NmraDcc.h>
#include "Config.h"
#include "AbstractSignal.h"
#include "Signal.h"
#include "Dimming.h"

NmraDcc dcc;

// id of the currently programmed signal * 2
// the LSB determines whether the main or distant signal is programmed.
// -1 = nothing currently programmed
int8_t programmingTarget = -1;
// Used while programming to cycle through the signal aspects.
uint8_t programmingTargetNextAspect;
// Used to restore the signal's aspect after programming.
uint8_t programmingTargetOldAspect;

volatile bool programmingButtonPressed = false;
volatile uint8_t seconds = 0;
volatile uint8_t quarterSeconds = 0;
volatile uint8_t debounceCnt = 0;

constexpr byte DCC_PIN_EXT_INT_NUM = 0;
constexpr byte DCC_PIN = 2;
constexpr byte LED_PIN = 13;
constexpr byte PROG_KEY = A6; // If changed, ADC configuration must also be changed!

DigitalPin<LED_PIN> ledPin;

Signal<PINS1> signal1 = Signal<PINS1>(0);
Signal<PINS2> signal2 = Signal<PINS2>(1);
Signal<PINS3> signal3 = Signal<PINS3>(2);
Signal<PINS4> signal4 = Signal<PINS4>(3);

AbstractSignal* const _signals[NUM_SIGNALS] = {&signal1, &signal2, &signal3, &signal4};

static void handleProgramming();
#if DEMO == 1
static void cycleAspects();
#elif DEMO == 2
static void randomAspects();
#endif

void setup() {
  dcc.pin(DCC_PIN_EXT_INT_NUM, DCC_PIN, true);
  dcc.init(MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0);

  ledPin.config(OUTPUT, LOW);

#if USE_SERIAL == 1
  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("#");
  Serial.println("# DCC Multiplex Decoder");
  Serial.println("#");
  Serial.println("# Author: Christian Flach");
  Serial.println("# Project URL: https://cmfcmf.github.io/modellbahn/decoder/multiplex");
  Serial.println("# Compiled at: " __DATE__ ", " __TIME__);
  Serial.println("#");

  Serial.println("");
  Serial.println("|");
  Serial.println("| Serial Interface Commands");
  Serial.println("|");
  Serial.println("| 1 up to 9: Set 1st to 9th signal aspect");
  Serial.println("| a up to d: Set only aspect of the 1st/2nd/3rd/4th signal");
  Serial.println("| x: Set aspects of all signals");
  Serial.println("| m: Toggle between setting aspects for both main and distant signal / only main signal / only distant signal");
  Serial.println("| sXXX (where XXX is an integer): Set address to XXX, works only after the programming button was pressed");
  Serial.println("|");
  Serial.println("");
#endif

  for (byte i = 0; i < NUM_SIGNALS; i++) {
    _signals[i]->begin();
  }

  #if DEMO == 2
  // Important: Use analogRead before we configure the ADC into free-running mode below
  randomSeed(analogRead(A7));
  #endif

  noInterrupts();

  // It is important to explicitly overwrite all registers, because the Arduino init code sets some to non-zero!

  /// Timer 1
  // Used for dimming up and down
  TCCR1A = (0 << WGM11) | (0 << WGM10); // CTC mode
  TCCR1B = (0 << CS12) | (1 << CS11) | (1 << CS10) // prescaler 64
    | (1 << WGM12); // CTC mode
  OCR1A = OCR1A_VALUE;
  TIMSK1 |= (1 << OCIE1A); // enable the CTC interrupt handler

  /// Timer 2
  // Used for charlieplexing the leds. It runs every 120us
  TCCR2A = (1 << WGM21) | (0 << WGM20); // CTC
  TCCR2B = (0 << WGM22) // CTC
    | (1 << CS22) | (0 << CS21) | (0 << CS20); // prescaler 64
  OCR2A = OCR2A_VALUE;
  TIMSK2 |= (1 << OCIE2A); // enable interrupt
  ASSR = 0; // disable async operation

  /// Watchdog
  // Used for rough timing, like on-board led blink frequency and button debouncing.
  // Enable the watchdog timer, but do not reset the controller on overflow.
  // Instead, trigger the WDT interrupt.
  // We never call wdt_reset() to make sure the watchdog is regularly triggered.
  MCUSR = 0;
  WDTCSR = (1 << WDCE) | (1 << WDE); // Allow changing other bits (for 4 cycles)
  WDTCSR = (1 << WDP2) | (0 << WDP1) | (0 << WDP0) // every 250ms
    | (1 << WDIE); // enable interrupt

  /// ADC
  // Used to read the programming button.
  ADMUX = (0 << REFS1) | (1 << REFS0) // AVCC as voltage reference
    | (1 << ADLAR) // left-adjust: upper 8 of 10 bits of ADC results stored in ADCH
    | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0); // select ADC6 (A6)
  ADCSRA = (1 << ADEN) // enable ADC
    | (1 << ADSC) // start first of infinite number of conversions
    | (1 << ADATE) // auto-trigger new conversions
    | (1 << ADIE) // enable interrupt
    // The ADC requires an input frequency between 50kHz and 200kHz.
    // This is achieved by configuring the ADC prescaler accordingly.
    // 16000000 / 200000 = 160 / 2 = 80 -> nearest prescaler values are 64 and 128
    // prescaler 128: 16000000/128 = 125000 (ok)
    // prescaler  64: 16000000/64  = 250000 (too high)
    | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC prescaler 128
  ADCSRB = (1 << ADTS2) | (0 << ADTS1) | (0 << ADTS0); // timer 0 overflow (~1000 times per second)

  interrupts();
}

ISR(WDT_vect) {
  const_cast<uint8_t &>(quarterSeconds)++; // Deliberately overflow
  if (const_cast<uint8_t &>(quarterSeconds) % 4 == 0) {
    seconds++; // Deliberately overflow
  }

  if (const_cast<uint8_t &>(debounceCnt) > 0) {
    const_cast<uint8_t &>(debounceCnt)--;
  }
}

template<byte LED> static inline void doCharlieplex(const byte & cur_dimm, byte & cur_led) {
    const CharlieLed led = AbstractSignal::leds[LED];
    const CharlieLed lastLed = AbstractSignal::leds[LED - 1 == -1 ? 11 : LED - 1];

    signal1.charlieplex(cur_dimm, LED, led.r, led.c, lastLed.r, lastLed.c);
    signal2.charlieplex(cur_dimm, LED, led.r, led.c, lastLed.r, lastLed.c);
    signal3.charlieplex(cur_dimm, LED, led.r, led.c, lastLed.r, lastLed.c);
    signal4.charlieplex(cur_dimm, LED, led.r, led.c, lastLed.r, lastLed.c);

    cur_led = LED + 1 == 12 ? 0 : LED + 1;
}

ISR(TIMER2_COMPA_vect) {
  static byte cur_dimm = 0;
  static byte cur_led = 0;

  switch (cur_led) {
    case 0:
      doCharlieplex<0>(cur_dimm, cur_led);
      return;
    case 1:
      doCharlieplex<1>(cur_dimm, cur_led);
      return;
    case 2:
      doCharlieplex<2>(cur_dimm, cur_led);
      return;
    case 3:
      doCharlieplex<3>(cur_dimm, cur_led);
      return;
    case 4:
      doCharlieplex<4>(cur_dimm, cur_led);
      return;
    case 5:
      doCharlieplex<5>(cur_dimm, cur_led);
      return;
    case 6:
      doCharlieplex<6>(cur_dimm, cur_led);
      return;
    case 7:
      doCharlieplex<7>(cur_dimm, cur_led);
      return;
    case 8:
      doCharlieplex<8>(cur_dimm, cur_led);
      return;
    case 9:
      doCharlieplex<9>(cur_dimm, cur_led);
      return;
    case 10:
      doCharlieplex<10>(cur_dimm, cur_led);
      return;
    case 11:
      doCharlieplex<11>(cur_dimm, cur_led);

      cur_dimm++;
      if (cur_dimm == DIMM_STEPS) {
        cur_dimm = 0;
      }
      return;
  }
}

ISR(TIMER1_COMPA_vect) {
  static uint8_t signalIdx = 0;
  if (signalIdx == 0) {
    doDimm1<0>();
    signalIdx = 1;
  } else if (signalIdx == 1) {
    doDimm2<0>();
    signalIdx = 2;
  } else if (signalIdx == 2) {
    doDimm3<0>();
    signalIdx = 3;
  } else {
    doDimm4<0>();
    signalIdx = 0;
  }
}

ISR(ADC_vect) {
  programmingButtonPressed = ADCH < ((1024 / 2) >> 2);
}

void loop() {
  dcc.process();

  handleProgramming();

  #if DEMO == 1
    cycleAspects();
  #elif DEMO == 2
    randomAspects();
  #endif
}

static void findNextProgrammingTarget() {
  if (programmingTarget >= 0) {
    const auto & signal = _signals[programmingTarget / 2];
    if (programmingTarget % 2 == 0) {
      signal->setAspect(programmingTargetOldAspect, false, true);
    } else {
      signal->setDistantAspect(programmingTargetOldAspect, false, 2);
    }
  }

  programmingTarget++;
  programmingTargetNextAspect = 0;
  for (; programmingTarget < NUM_SIGNALS * 2; programmingTarget++) {
    const auto & signal = _signals[programmingTarget / 2];
    if (programmingTarget % 2 == 0 && signal->isSignalConnected()) {
      programmingTargetOldAspect = signal->getAspect();
      return;
    }
    if (programmingTarget % 2 == 1 && signal->isAdditionalDistantSignalConnected()) {
      programmingTargetOldAspect = signal->getDistantAspect();
      return;
    }
  }
  // No signal left, stop programming.
  programmingTarget = -1;
}

static void handleProgramming() {
  if (programmingButtonPressed && debounceCnt == 0) {
    findNextProgrammingTarget();
    debounceCnt = 2;
    if (programmingTarget == -1) {
      ledPin.low();
    }
  }

  if (programmingTarget >= 0) {
    ledPin.write(quarterSeconds & (1 << 0));

    static uint8_t lastSeconds = 0;
    uint8_t currentSeconds = seconds; // use a local variable to avoid loading it twice due to its volatile modifier.
    if (lastSeconds != currentSeconds) {
      lastSeconds = currentSeconds;
      const auto & signal = _signals[programmingTarget / 2];
      if (programmingTarget % 2 == 0) {
        signal->setAspect(programmingTargetNextAspect, false, true);
        programmingTargetNextAspect = (programmingTargetNextAspect + 1) % signal->getNumAspects();
      } else {
        signal->setDistantAspect(programmingTargetNextAspect, false, 1);
        programmingTargetNextAspect = (programmingTargetNextAspect + 1) % signal->getNumDistantAspects();
      }
    }
  }
}

/**
 * address: 1 to x
 * direction: 0 or 1 (= red or green)
 */
void notifyDccAccTurnoutOutput(uint16_t address, uint8_t direction, uint8_t power) {
  if (!power) {
    // Ignore turn-off commands
    return;
  }

  if (programmingTarget >= 0) {
    // We re-use the button's debounceCnt variable for debouncing DCC packets.
    // A controller often sends the same packet multiple times.
    // That is why we need let some time pass after changing the programming
    // target.
    if (debounceCnt > 0) {
      return;
    }
    debounceCnt = 3;

    const auto & signal = _signals[programmingTarget / 2];
    if (programmingTarget % 2 == 0) {
      signal->setMainAddress(address);
    } else {
      signal->setDistantAddress(address);
    }
    findNextProgrammingTarget();
    return;
  }

  for (uint8_t i = 0; i < NUM_SIGNALS; i++) {
    const auto & signal = _signals[i];
    const auto & minMainAddress = signal->getMainAddress();
    const auto & maxMainAddress = minMainAddress + (signal->getNumAspects() + 1) / 2;
    if (address >= minMainAddress && address < maxMainAddress) {
      signal->setAspect((address - minMainAddress) * 2 + direction);
    }
    const auto & minDistantAddress = signal->getDistantAddress();
    const auto & maxDistantAddress = minDistantAddress + (signal->getNumDistantAspects() + 1) / 2;
    if (address >= minDistantAddress && address < maxDistantAddress) {
      signal->setDistantAspect((address - minDistantAddress) * 2 + direction);
    }
  }
}

#if USE_SERIAL == 1
void serialEvent() {
  static int8_t _signal = -1;
  static int8_t mode = 0;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == 's' && programmingTarget >= -1) {
      uint16_t newAddress = Serial.parseInt();
      if (newAddress > 0) {
        const auto & signal = _signals[programmingTarget / 2];
        if (programmingTarget % 2 == 0) {
          signal->setMainAddress(newAddress);
        } else {
          signal->setDistantAddress(newAddress);
        }
        findNextProgrammingTarget();
      }
    }
    if (inChar >= 'a' && inChar <= 'a' + NUM_SIGNALS) {
      inChar -= 'a';
      _signal = inChar;
    } else if (inChar == 'x') {
      _signal = -1;
    } else if (inChar == 'm') {
      mode++;
      if (mode == 2) {
        mode = -1;
      }
    } else if (inChar >= '1' && inChar <= '9') {
      inChar -= '1';
      if (_signal == -1) {
        for (byte i = 0; i < NUM_SIGNALS; i++) {
          if (mode >= 0) {
            _signals[i]->setAspect(inChar);
          }
          if (mode <= 0) {
            _signals[i]->setDistantAspect(inChar);
          }
        }
      } else {
        if (mode >= 0) {
          _signals[_signal]->setAspect(inChar);
        }
        if (mode <= 0) {
          _signals[_signal]->setDistantAspect(inChar);
        }
      }
    }
  }
}
#endif

#if DEMO == 1
static void cycleAspects() {
  static byte nextAspect = 0;
  static byte nextDistantAspect = 0;
  static uint8_t lastSeconds = 0;
  uint8_t currentSeconds = seconds; // use a local variable to avoid loading it twice due to its volatile modifier.
  if (lastSeconds != currentSeconds) {
    lastSeconds = currentSeconds;
    for (byte s = 0; s < NUM_SIGNALS; s++) {
      _signals[s]->setAspect(nextAspect, false);
      if (_signals[s]->isAdditionalDistantSignalConnected()) {
          _signals[s]->setDistantAspect(nextDistantAspect, false);
      }
    }

    nextAspect = (nextAspect + 1) % 5;
    nextDistantAspect = (nextDistantAspect + 1) % 4;
  }
}
#elif DEMO == 2
static void randomAspects() {
  static uint8_t lastSeconds = 0;
  uint8_t currentSeconds = seconds; // use a local variable to avoid loading it twice due to its volatile modifier.
  if (lastSeconds != currentSeconds) {
    lastSeconds = currentSeconds;
    for (byte s = 0; s < NUM_SIGNALS; s++) {
      _signals[s]->setRandomAspect();
    }
  }
}
#endif
