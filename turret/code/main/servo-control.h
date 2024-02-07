#ifndef __SERVO_CONTROL_H__
#define __SERVO_CONTROL_H__

#include <Servo.h>

#define MIN(VALUE, CONSTRAIN) (((VALUE) > (CONSTRAIN)) ? (VALUE) : (CONSTRAIN))
#define MAX(VALUE, CONSTRAIN) (((VALUE) > (CONSTRAIN)) ? (CONSTRAIN) : (VALUE))

class GunAngle {
public:
  GunAngle(Servo& servo) {
    _servo = &servo;
  }

  void write(uint8_t value) {
    if (value >= 0x01 && value <= 0x7F) {
      _speed = map(value, 0x01, 0x7F, 200, 10);
      _step = 1;
    } else if (value >= 0x80 && value <= 0xFE) {
      _speed = map(value, 0x80, 0xFE, 200, 10);
      _step = -1;
    } else {
      _step = 0;
      _speed = 0;
    }
  }

  void set(uint8_t value) {
    _servo->write(value);
  }
  void lock() {
    _servo->write(100);
    _lock = true;
  }

  void unlock() {
    _lock = false;
  }

  void tick() {
    if (_lock == false && millis() - _time_buffer >= _speed) {
      _angle = MAX(MIN(_angle + _step, 0), 180);
      _servo->write(_angle);
      _time_buffer = millis();
    }
  }

private:
  Servo* _servo;
  uint16_t _speed = 0;
  int8_t _step = 0;
  int16_t _angle = 90;
  bool _lock = false;
  uint32_t _time_buffer = 0;
};

class CommanderServo {
public:
  CommanderServo(Servo& servo) {
    _servo = &servo;
  }

  CommanderServo(Servo& servo, const uint8_t min_angle, const uint8_t max_agle) {
    _servo = &servo;
    _min_angle = min_angle;
    _max_angle = max_agle;
  }

  void write(uint8_t value) {
    _static_position = false;
    if (value >= 0x01 && value <= 0x7F) {
      _speed = map(value, 0x01, 0x7F, 200, 10);
      _step = 1;
    } else if (value >= 0x80 && value <= 0xFE) {
      _speed = map(value, 0x80, 0xFE, 200, 10);
      _step = -1;
    } else {
      _step = 0;
      _speed = 0;
    }
  }

  void set(uint8_t value) {
    _static_position = true;
    _angle = map(value, 0x00, 0xFF, _min_angle, _max_angle);
    _servo->write(_angle);
  }

  void tick() {
    if (_static_position == false && millis() - _time_buffer >= _speed) {
      _angle = MAX(MIN(_angle + _step, _min_angle), _max_angle);
      _servo->write(_angle);
      _time_buffer = millis();
    }
  }

private:
  Servo* _servo;
  uint8_t _speed = 0;
  int8_t _step = 0;
  int16_t _angle = 90;
  uint32_t _time_buffer = 0;
  bool _static_position = false;
  uint8_t _min_angle = 0;
  uint8_t _max_angle = 180;
};

#endif