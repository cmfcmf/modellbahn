#pragma GCC optimize ("O3")
#pragma GCC optimize ("unroll-loops")

#include <NmraDcc.h>
#include "Config.h"
#include "AbstractSignal.h"
#include "Signal.h"
#include <EEPROM.h>

NmraDcc dcc;

bool programming = false;

constexpr unsigned int EE_ADDRESS = 42;

constexpr byte DCC_PIN_EXT_INT_NUM = 0;
constexpr byte DCC_PIN = 2;
constexpr byte LED_PIN = 13;
constexpr byte PROG_KEY = A6; // If changed, ADC configuration must also be changed!

DigitalPin<LED_PIN> ledPin;

constexpr byte NUM_SIGNALS = 4;

#define PINS1  3,  4,  5,  6
#define PINS2  7,  8,  9, 10
#define PINS3 12, 11, A0, A1
#define PINS4 A2, A3, A4, A5

Signal<PINS1>* signal1 = new Signal<PINS1>();
Signal<PINS2>* signal2 = new Signal<PINS2>();
Signal<PINS3>* signal3 = new Signal<PINS3>();
Signal<PINS4>* signal4 = new Signal<PINS4>();

AbstractSignal* _signals[NUM_SIGNALS] = {signal1, signal2, signal3, signal4};

void handleProgramming();
uint16_t getAddress();
void setAddress(uint16_t);
void clearSavedState();
#if DEMO == 1
void cycleAspects();
#elif DEMO == 2
void randomAspects();
#endif

void setup() {
  dcc.pin(DCC_PIN_EXT_INT_NUM, DCC_PIN, true);
  dcc.init(MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0);

  ledPin.config(OUTPUT, LOW);

  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("#===========================#");
  Serial.println("# Starting...               #");
  Serial.print  ("# Address: ");
  const uint16_t address = getAddress();
  Serial.print(address, DEC);
  if (address < 10000) {
    Serial.print(' ');
  }
  if (address < 1000) {
    Serial.print(' ');
  }
  if (address < 100) {
    Serial.print(' ');
  }
  if (address < 10) {
    Serial.print(' ');
  }
  Serial.println("            #");
  Serial.println("#===========================#");

  for (byte i = 0; i < NUM_SIGNALS; i++) {
    _signals[i]->begin();
  }

  #if DEMO == 2
  // Important: Use analogRead before we configure the ADC into free-running mode below
  randomSeed(analogRead(A7));
  #endif

  noInterrupts();

  /// Timer 1
  // Used for dimming up and down, it also triggers ADC measurements (configured below)
  TCCR1A = (0 << WGM11) | (0 << WGM10); // CTC mode
  TCCR1B = (1 << CS12) | (0 << CS11) | (0 << CS10) // prescaler 256
    | (1 << WGM12); // CTC mode
  OCR1A = OCR1A_VALUE;
  TIMSK1 |= (1 << OCIE1A); // enable the CTC interrupt handler

  /// Timer 2
  // Used for charlieplexing the leds. It runs every 120us

  // It is important to explicitly overwrite all registers, because the Arduino init code sets some to non-zero!
  TCCR2A = (1 << WGM21) | (0 << WGM20); // CTC
  TCCR2B = (0 << WGM22) // CTC
    | (1 << CS22) | (0 << CS21) | (0 << CS20); // prescaler 64
  OCR2A = OCR2A_VALUE;
  TIMSK2 |= (1 << OCIE2A); // enable interrupt
  ASSR = 0; // disable async operation

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

template<byte LED> static inline void doCharlieplex(const byte & cur_dimm, byte & cur_led) {
    const charlieLed led = AbstractSignal::leds[LED];
    const charlieLed lastLed = AbstractSignal::leds[LED - 1 == -1 ? 11 : LED - 1];

    signal1->charlieplex(cur_dimm, LED, led.r, led.c, lastLed.r, lastLed.c);
    signal2->charlieplex(cur_dimm, LED, led.r, led.c, lastLed.r, lastLed.c);
    signal3->charlieplex(cur_dimm, LED, led.r, led.c, lastLed.r, lastLed.c);
    signal4->charlieplex(cur_dimm, LED, led.r, led.c, lastLed.r, lastLed.c);

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

__attribute__((always_inline)) __attribute__((optimize("O3"))) __attribute__((optimize("unroll-loops")))
inline void dimm() {
  uint16_t mask = 1;
  for (byte ledIdx = 0; ledIdx < NUM_LEDS; ledIdx++) {
    for (byte signalIdx = 0; signalIdx < NUM_SIGNALS; signalIdx++) {
      _signals[signalIdx]->dimm(ledIdx, mask);
    }
    mask <<= 1;
  }
}

volatile uint8_t blinkCnt = 0;
volatile uint8_t debounceCnt = 0;

ISR(TIMER1_COMPA_vect) {
  interrupts();

  dimm();

  blinkCnt++; // Deliberately overflow
  if (debounceCnt > 0) {
    debounceCnt--;
  }
}

volatile bool programmingButtonPressed = false;

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

void serialEvent() {
  static int8_t _signal = -1;
  static int8_t mode = 0;
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == 's') {
      uint16_t newAddress = Serial.parseInt();
      if (newAddress > 0) {
        setAddress(newAddress);
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
    } else if (inChar >= '0' && inChar <= '9') {
      inChar -= '0';
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

void handleProgramming() {
  if (programmingButtonPressed && debounceCnt == 0) {
    programming = !programming;
    debounceCnt = 50;
    if (!programming) {
      ledPin.low();
    }
  }

  if (programming) {
    ledPin.write(blinkCnt & (1 << 4));
  }
}

void setAspect(byte signalIdx, byte aspectIdx) {
  byte i = 0;
  for (byte s = 0; s < NUM_SIGNALS; s++) {
    if (i == signalIdx) {
      _signals[s]->setAspect(aspectIdx);

      Serial.print("SIGNAL ");
      Serial.print(signalIdx);
      Serial.print(" set to ");
      Serial.println(aspectIdx);
    }
    i++;
    if (_signals[s]->isAdditionalDistantSignalConnected()) {
      if (i == signalIdx) {
        _signals[s]->setDistantAspect(aspectIdx);

        Serial.print("SIGNAL ");
        Serial.print(signalIdx);
        Serial.print(" set to ");
        Serial.println(aspectIdx);
      }
      i++;
    }
  }
}

void clearSavedState() {
  for (byte i = 0; i < NUM_SIGNALS; i++) {
    _signals[i]->clearSavedState();
  }
}

void setAddress(uint16_t address) {
  Serial.print("Setting address to ");
  Serial.println(address);

  EEPROM.update(EE_ADDRESS, address);
  clearSavedState();
}

uint16_t getAddress() {
  return EEPROM.read(EE_ADDRESS);
}

// This function is called whenever a normal DCC Turnout Packet is received
void notifyDccAccState(__attribute__((unused)) uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, __attribute__((unused)) uint8_t State) {
  if (programming) {
    setAddress(BoardAddr);
    programming = false;

    return;
  }

  uint16_t addr = getAddress();
  if (BoardAddr < addr) {
    return;
  }
  uint16_t signalIdx = BoardAddr - addr;
  byte aspectIdx = OutputAddr;
  setAspect(signalIdx, aspectIdx);
}

#if DEMO == 1
void cycleAspects() {
  static unsigned long lastSwitch = 0;
  static byte nextAspect = 0;
  static byte nextDistantAspect = 0;
  if (millis() - lastSwitch > 1000) {
    lastSwitch = millis();
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
void randomAspects() {
  static unsigned long lastSwitch = 0;
  if (millis() - lastSwitch > 1000) {
    lastSwitch = millis();
    for (byte s = 0; s < NUM_SIGNALS; s++) {
      _signals[s]->setRandomAspect();
    }
  }
}
#endif