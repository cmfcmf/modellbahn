#include "AbstractSignal.h"
#include "Config.h"
#include <EEPROM.h>

#pragma GCC optimize ("O3")
#pragma GCC optimize ("unroll-loops")

AbstractSignal::AbstractSignal() {
  m_id = AbstractSignal::nextId++;
}

AbstractSignal* AbstractSignal::begin() {
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
  Serial.println(",---------------------------.");
  Serial.print(  "| Signal   #");
  Serial.print(m_id);
  Serial.println("               |");
  detectMainSignal();
  detectDistantSignal();
  Serial.println("`---------------------------'");
}

bool AbstractSignal::isSignalConnected(void) {
  return m_mapping != NULL;
}
bool AbstractSignal::isAdditionalDistantSignalConnected(void) {
  return m_distantMapping != NULL;
}

void AbstractSignal::clearSavedState() {
  EEPROM.update(m_id * 2,     0);
  EEPROM.update(m_id * 2 + 1, 0);
}

void AbstractSignal::saveState() {
  EEPROM.update(m_id * 2,     m_currentAspectIndex);
  EEPROM.update(m_id * 2 + 1, m_currentDistantAspectIndex);
}

void AbstractSignal::applySavedState() {
  byte aspectId;
  byte distantAspectId;
  EEPROM.get(m_id * 2, aspectId);
  EEPROM.get(m_id * 2 + 1, distantAspectId);
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

void AbstractSignal::setAspect(byte aspectId, bool persist) {
  if (aspectId >= m_numAspects) {
    return;
  }

  m_currentAspectIndex = aspectId;
  m_currentAspect = m_aspects[m_mapping[aspectId]];

  if (persist) {
    saveState();
  }

  if (!m_currentAspect.darkenDistantSignal) {
    // Apply distant signal aspect again in case it was darkened
    applyAspectChange(m_currentDistantAspect);
  }

  applyAspectChange(m_currentAspect);
  if (m_currentAspect.darkenDistantSignal) {
    // Darken distant signal, but don't overwrite m_currentDistantAspect
    applyAspectChange(m_aspects[VR_DARK]);
  }
}

void AbstractSignal::setDistantAspect(byte distantAspectId, bool persist) {
  /*
  Serial.print("Trying to set distant signal to #");
  Serial.print(distantAspectId);
  Serial.print(" of ");
  Serial.print(m_numDistantAspects);
  Serial.println(".");
  */
  if (distantAspectId >= m_numDistantAspects) {
    return;
  }

  m_currentDistantAspectIndex = distantAspectId;
  m_currentDistantAspect = m_aspects[m_distantMapping[distantAspectId]];

  if (persist) {
    saveState();
  }

  if (!m_currentAspect.darkenDistantSignal) {
    // Only dimm to new aspect if the distant signal currently isn't darkened.
    applyAspectChange(m_currentDistantAspect);
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

byte AbstractSignal::nextId = 0;
