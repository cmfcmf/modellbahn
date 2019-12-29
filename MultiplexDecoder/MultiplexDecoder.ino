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

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

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


  Serial.print("CHARLIE_CYCLE_LENGTH_US: "); Serial.println(CHARLIE_CYCLE_LENGTH_US);
  Serial.print("DIMM_STEPS: "); Serial.println(DIMM_STEPS);
  Serial.print("DIMM_TIME_US: "); Serial.println(DIMM_TIME_US);
  Serial.print("PRESCALER: "); Serial.println(PRESCALER);
  Serial.print("OCR1A_VALUE: "); Serial.println(OCR1A_VALUE);
  // Serial.print("DIMM_CALLING_INTERVAL_US: "); Serial.print(DIMM_CALLING_INTERVAL_US / 0xffffffff); Serial.print(DIMM_CALLING_INTERVAL_US % 0xffffffff);
  Serial.print("DIMM_UP_TOGGLE_DIMM_DIRECTION_IN: "); Serial.println(DIMM_UP_TOGGLE_DIMM_DIRECTION_IN);

  #if DEMO == 2
  // Important: Use analogRead before we configure the ADC into free-running mode below
  randomSeed(analogRead(A7));
  #endif

  noInterrupts();

  /// Timer 1

  // Important to explicitly set this to 0, because the Arduino init code sets it to non-zero!
  TCCR1A = 0;

  if (PRESCALER == 64) {
    TCCR1B = (1 << CS11) | (1 << CS10);
  } else {
    // Make sure we do not accidentally supply an invalid prescaler.
    while (true) {};
  }
  TCCR1B |= (1 << WGM12) /* CTC */;

  OCR1A = OCR1A_VALUE;
  TIMSK1 |= 1 << OCIE1A;

  /// ADC

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
    // prescaler  64: 16000000     = 250000 (too high)
    | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC prescaler 128
  ADCSRB = (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0); // free running mode

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

ISR(TIMER1_COMPA_vect) {
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

volatile bool programmingButtonPressed = false;

ISR(ADC_vect) {
  programmingButtonPressed = ADCH < ((1024 / 2) >> 2);
}

void loop() {
  dcc.process();

  handleProgramming();

  static unsigned long lastDimm = 0;
  if (millis() - lastDimm > DIMM_CALLING_INTERVAL_US / 1000) {
    lastDimm = millis();
    for (byte i = 0; i < NUM_SIGNALS; i++) {
      _signals[i]->dimm();
    }
  }

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
  static unsigned long lastPress = 0;

  if (programmingButtonPressed && millis() - lastPress > 500) {
    lastPress = millis();
    programming = !programming;
    if (programming) {
      clearSavedState();
    }
  }

  if (programming) {
    static unsigned long lastBlink = 0;
    static bool on = false;
    if (millis() - lastBlink > 200) {
      digitalWrite(LED_PIN, on = !on);
      lastBlink = millis();
    }
  } else {
    digitalWrite(LED_PIN, LOW);
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
      _signals[s]->setAspect(nextAspect);
      if (_signals[s]->isAdditionalDistantSignalConnected()) {
          _signals[s]->setDistantAspect(nextDistantAspect);
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