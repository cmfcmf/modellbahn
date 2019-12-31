#include "AbstractSignal.h"
#include "Config.h"
#include <EEPROM.h>

#pragma GCC optimize ("O3")
#pragma GCC optimize ("unroll-loops")

AbstractSignal::AbstractSignal(const byte id) {
  m_id = id;
}

AbstractSignal* AbstractSignal::begin() {
  uint16_t addressMain;
  uint16_t addressDistant;
  EEPROM.get(m_id * SIGNAL_EEPROM_SIZE + 2, addressMain);
  if (addressMain >= 2048) {
    addressMain = 1;
  }
  m_addressMain = addressMain;
  EEPROM.get(m_id * SIGNAL_EEPROM_SIZE + 4, addressDistant);
  if (addressDistant >= 2048) {
    addressDistant = 4;
  }
  m_addressDistant = addressDistant;

  detect();

  for (byte i = 0; i < NUM_LEDS; i++) {
    // Start all leds off.
    m_leds[i].dimmValue             = 0;
    m_leds[i].toggleDimmDirectionIn = 0;
  }
  m_ledState = 0;

  applySavedState();

  return this;
}

void AbstractSignal::detect() {
#if USE_SERIAL == 1
  Serial.println("");
  Serial.print(  "| Signal   #"); Serial.println(m_id);
  Serial.print(  "| Address (main): "); Serial.println(m_addressMain);
  Serial.print(  "| Address (distant): "); Serial.println(m_addressDistant);
#endif
  detectMainSignal();
  detectDistantSignal();
}

uint16_t AbstractSignal::getMainAddress() {
  return m_addressMain;
}

uint16_t AbstractSignal::getDistantAddress() {
  return m_addressDistant;
}

void AbstractSignal::setMainAddress(uint16_t address) {
  m_addressMain = address;
  EEPROM.put(m_id * SIGNAL_EEPROM_SIZE + 2, address);
}

void AbstractSignal::setDistantAddress(uint16_t address) {
  m_addressDistant = address;
  EEPROM.put(m_id * SIGNAL_EEPROM_SIZE + 4, address);
}

uint8_t AbstractSignal::getAspect(void) {
  return m_currentAspectIndex;
}

uint8_t AbstractSignal::getDistantAspect(void) {
  return m_currentDistantAspectIndex;
}


uint8_t AbstractSignal::getNumAspects() {
  return m_numAspects;
}

uint8_t AbstractSignal::getNumDistantAspects() {
  return m_numDistantAspects;
}

bool AbstractSignal::isSignalConnected(void) {
  return m_mapping != NULL;
}
bool AbstractSignal::isAdditionalDistantSignalConnected(void) {
  return m_distantMapping != NULL;
}

void AbstractSignal::clearSavedState() {
  EEPROM.update(m_id * SIGNAL_EEPROM_SIZE,     0);
  EEPROM.update(m_id * SIGNAL_EEPROM_SIZE + 1, 0);
}

void AbstractSignal::saveState() {
  EEPROM.update(m_id * SIGNAL_EEPROM_SIZE,     m_currentAspectIndex);
  EEPROM.update(m_id * SIGNAL_EEPROM_SIZE + 1, m_currentDistantAspectIndex);
}

void AbstractSignal::applySavedState() {
  byte aspectId;
  byte distantAspectId;
  EEPROM.get(m_id * SIGNAL_EEPROM_SIZE, aspectId);
  EEPROM.get(m_id * SIGNAL_EEPROM_SIZE + 1, distantAspectId);
  if (aspectId >= m_numAspects) {
    aspectId = 0;
  }
  if (distantAspectId >= m_numDistantAspects) {
    distantAspectId = 0;
  }

  setAspect(aspectId);
  setDistantAspect(distantAspectId);
}

#if DEMO == 2
void AbstractSignal::setRandomAspect(void) {
    setAspect(random(m_numAspects), false);
    if (isAdditionalDistantSignalConnected()) {
      setDistantAspect(random(m_numDistantAspects), false);
    }
}
#endif

void AbstractSignal::setAspect(byte aspectId, bool persist, bool ignoreDistantSignal) {
  if (aspectId >= m_numAspects) {
    return;
  }

  m_currentAspectIndex = aspectId;
  m_currentAspect = m_aspects[m_mapping[aspectId]];

  if (persist) {
    saveState();
  }

  if (!m_currentAspect.darkenDistantSignal && !ignoreDistantSignal) {
    // Apply distant signal aspect again in case it was darkened
    applyAspectChange(m_currentDistantAspect);
  }

  applyAspectChange(m_currentAspect);
  if (m_currentAspect.darkenDistantSignal && !ignoreDistantSignal) {
    // Darken distant signal, but don't overwrite m_currentDistantAspect
    applyAspectChange(m_aspects[VR_DARK]);
  }
}

void AbstractSignal::setDistantAspect(byte distantAspectId, bool persist, uint8_t debug) {
  if (distantAspectId >= m_numDistantAspects) {
    return;
  }

  m_currentDistantAspectIndex = distantAspectId;
  m_currentDistantAspect = m_aspects[m_distantMapping[distantAspectId]];

  if (persist) {
    saveState();
  }

  if (!m_currentAspect.darkenDistantSignal || debug == 1) {
    // Only dimm to new aspect if the distant signal currently isn't darkened.
    applyAspectChange(m_currentDistantAspect);
  } else if (m_currentAspect.darkenDistantSignal && debug == 2) {
    // If the distant signal is currently darkened, we normally do not need to
    // make it dark again. This functionality is only used when cycling aspects while programming.
    applyAspectChange(m_aspects[VR_DARK]);
  }
}

void AbstractSignal::applyAspectChange(AspectMapping aspect) {
  uint16_t ledBitmask = aspect.leds;
  uint16_t validLedsBitmask = aspect.validLeds;
  uint16_t mask = 1;

  for (byte led = 0; led < NUM_LEDS; led++) {
    if (mask & validLedsBitmask) {
      // The current led is a valid led of this aspect.
      // This is used so that main signal aspects do not affect the
      // leds of the pre signal.
      if (ledBitmask & mask) {
        // The current led is supposed to be on in the new aspect.
        if (m_ledState & mask) {
          // The current led is already on (due to the last aspect)
          // -> Do nothing, simply keep it on.
        } else {
          // The current led was not on in the last aspect
          // However, we must not immediately start dimming it up.
          // We first need to wait for all other leds that need dimm
          // down to turn off. That is why we set the rest.

          m_leds[led].toggleDimmDirectionIn = DIMM_UP_TOGGLE_DIMM_DIRECTION_IN;
          m_ledState &= ~mask;  // Immediately start to dimm down
        }
      } else {
        // The current led is supposed to be off in the new aspect.

        m_leds[led].toggleDimmDirectionIn = 0;
        m_ledState &= ~mask;  // Immediately start to dimm down
      }
    }
    mask = mask << 1L;
  }
}
