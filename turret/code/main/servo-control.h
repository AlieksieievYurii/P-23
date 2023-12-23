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

  void tick() {
    if (millis() - _time_buffer >= _speed) {
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
  uint32_t _time_buffer = 0;
};

class CommanderServo {
public:
  CommanderServo(Servo& servo) {
    _servo = &servo;
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
    _angle = map(value, 0x00, 0xFF, 0, 180);
    _servo->write(_angle);
  }

  void tick() {
    if (_static_position == false && millis() - _time_buffer >= _speed) {
      _angle = MAX(MIN(_angle + _step, 0), 180);
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
};

#endif