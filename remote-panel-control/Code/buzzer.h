#ifndef __BUZZER_H__
#define __BUZZER_H__

#define BEEP_DURATION 100
#define BEEP_ERROR_DURATION 500
#define BEEP_WARNING_DURATION 300

#include "config.h"

class Buzzer {
public:
  Buzzer(uint8_t buzzer_pin) {
    _buzzer_pin = buzzer_pin;
  }
  void beep() {
    digitalWrite(_buzzer_pin, HIGH);
    _beep_timestamp = millis();
  }

  void error() {
    _error = true;
  }

  void warning(bool activate) {
    _warning = activate;
  }

  void tick() {
    if (_error) {
      if (millis() - _beep_timestamp > BEEP_ERROR_DURATION) {
        _beep_state = !_beep_state;
        digitalWrite(_buzzer_pin, _beep_state);
        _beep_timestamp = millis();
      }
    } else if (_warning) {
        if (millis() - _beep_timestamp > BEEP_WARNING_DURATION) {
        _beep_state = !_beep_state;
        digitalWrite(_buzzer_pin, _beep_state);
        _beep_timestamp = millis();
      }
    } else {
      if (millis() - _beep_timestamp > BEEP_DURATION) {
        digitalWrite(_buzzer_pin, LOW);
      }
    }
  }
private:
  uint8_t _buzzer_pin;
  uint32_t _beep_timestamp = 0;
  bool _beep_state = false;
  bool _error = false;
  bool _warning = false;
};

Buzzer BuzzerInstance(BUZZER);

#endif