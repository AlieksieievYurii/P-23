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
    if (value >= 135 && value <= 255) {
      _speed = map(value, 135, 255, 200, 10);
      _step = 1;
    } else if (value >= 0 && value <= 124) {
      _speed = map(value, 124, 0, 200, 10);
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
  int8_t _speed = 0;
  int _step = 0;
  uint8_t _angle = 90;
  uint32_t _time_buffer = 0;
};

#endif