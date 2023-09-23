#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <Arduino.h>

#define UPDATE_FREQUENCY 500

#define BUF_SIZE 10

#define MAX_INPUT_VOLTAGE 20.0
#define MAP_VOLTAGE(VALUE) VALUE* MAX_INPUT_VOLTAGE / 1023.0

#define MAX_BATTERY_VOLTAGE 16.0
#define MIN_BATTERY_VOLTAGE 14.5

#define MAX(VALUE, CONSTRAIN) (((VALUE) > (CONSTRAIN)) ? (CONSTRAIN) : (VALUE))

#define MAP_BATTERY_CAPACITY(VOLTAGE) MAX(round((VOLTAGE - MIN_BATTERY_VOLTAGE) * 4.0 / (MAX_BATTERY_VOLTAGE - MIN_BATTERY_VOLTAGE)), 4);

class Battery {
public:
  volatile float voltage = 0;
  volatile uint8_t voltage_intpart = 0;
  volatile uint8_t voltage_decpart = 0;
  volatile uint8_t capacity = 4;

  Battery(uint8_t voltage_pin) {
    _voltage_pin = voltage_pin;
  }

  void init() {
    for (uint8_t i = 0; i < BUF_SIZE; i++)
      _buffer[i] = MAP_VOLTAGE(analogRead(_voltage_pin));
  }


  void tick() {
    if (millis() - _last_update_time >= UPDATE_FREQUENCY) {
      // --------------- Voltage Calculation -------------------
      _buffer[_index++] = MAP_VOLTAGE(analogRead(_voltage_pin));
      if (_index >= BUF_SIZE)
        _index = 0;
      float sum = 0;
      for (uint8_t i = 0; i < BUF_SIZE; i++)
        sum += _buffer[i];

      voltage = sum / BUF_SIZE;
      voltage_intpart = (uint8_t)voltage;
      voltage_decpart = (uint16_t)(voltage * 100) % 100;
      // --------------------------------------------------------

      // ------------- Capacity Calculation ---------------------
      if (_battery_verification_start_time != 0) {
        if (millis() - _battery_verification_start_time >= 5000) {
          capacity = _last_battery_indicator;
          _battery_verification_start_time = 0;
        }
      } else {
        _last_battery_indicator = MAP_BATTERY_CAPACITY(voltage);

        if (_last_battery_indicator != capacity)
          _battery_verification_start_time = millis();
      }

      _last_update_time = millis();
    }
  }

private:
  uint8_t _voltage_pin;
  uint32_t _last_update_time = 0;
  uint8_t _index = 0;
  uint32_t _battery_verification_start_time = 0;
  uint8_t _last_battery_indicator = 0;
  float _buffer[BUF_SIZE] = { 0 };
};

#endif