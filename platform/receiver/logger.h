#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <Arduino.h>

#define BLINKING_FREQUENCY 500

enum LoggerState : byte {
  NONE,
  TRANSCEIVER_INIT_ERROR,
  TRANSCEIVER_SET_CHANEL_ERROR,
  TRANSCEIVER_SET_RF_ERROR
};

class Logger {
public:
  Logger(uint8_t led_pin, uint8_t buzzer_pin, HardwareSerial& serial_stream) {
    _led_pin = led_pin;
    _buzzer_pin = buzzer_pin;
    _serial_stream = &serial_stream;
  }

  void set_state(LoggerState state) {
    _logger_state = state;
    _message_send = false;
  }

  void tick() {
    if (_logger_state == NONE)
      return;

    if (millis() - _last_tick >= BLINKING_FREQUENCY) {
      _blink_state = !_blink_state;
      digitalWrite(_buzzer_pin, _blink_state);
      digitalWrite(_led_pin, _blink_state);
      _last_tick = millis();
    }

#ifdef DEBUG
    if (_message_send != true) {
      switch (_logger_state) {
        case TRANSCEIVER_INIT_ERROR:
          Serial.println("[Error] Transceiver Initialization Failed!");
          break;
      }

      _message_send = true;
    }
#endif
  }

  // void test() {
  //   uint32_t m = millis();

  //   if (m - _last_tick >= 2200) {
  //     _last_tick = millis();
  //   } else if (m - _last_tick >= 1900) {
  //     digitalWrite(_buzzer_pin, 0);
  //     digitalWrite(_led_pin, 0);
  //   } else if (m - _last_tick >= 1600) {
  //     digitalWrite(_buzzer_pin, 1);
  //     digitalWrite(_led_pin, 1);
  //   } else if (m - _last_tick >= 1300) {
  //     digitalWrite(_buzzer_pin, 0);
  //     digitalWrite(_led_pin, 0);
  //   } else if (m - _last_tick >= 1000) {
  //     digitalWrite(_buzzer_pin, 1);
  //     digitalWrite(_led_pin, 1);
  //   }
  // }

private:
  uint8_t _led_pin, _buzzer_pin;
  LoggerState _logger_state;
  HardwareSerial* _serial_stream;
  uint32_t _last_tick = 0;
  bool _blink_state = false;
  bool _message_send = true;
};

#endif