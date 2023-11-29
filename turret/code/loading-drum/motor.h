#ifndef __MOTOR_H__
#define __MOTOR_H__

enum State {
  NONE,
  OPEN,
  CLOSE
};

class ConstrainedMotor {
public:
  ConstrainedMotor(uint8_t motor_forward_pin, uint8_t motor_backward_pin, uint8_t close_end_pin, uint8_t open_end_pin) {
    _motor_forward_pin = motor_forward_pin;
    _motor_backward_pin = motor_backward_pin;
    _close_end_pin = close_end_pin;
    _open_end_pin = open_end_pin;
  }

  void open() {
    _open_state = OPEN;
  }

  void close() {
    _open_state = CLOSE;
  }

  void tick() {
    if (_open_state == OPEN) {
      if (analogRead(_open_end_pin) < 500) {
        digitalWrite(_motor_forward_pin, LOW);
        digitalWrite(_motor_backward_pin, HIGH);
      } else {
        digitalWrite(_motor_forward_pin, HIGH);
        digitalWrite(_motor_backward_pin, HIGH);
      }
    } else if (_open_state == CLOSE) {
      if (analogRead(_close_end_pin) < 500) {
        digitalWrite(_motor_forward_pin, HIGH);
        digitalWrite(_motor_backward_pin, LOW);
      } else {
        digitalWrite(_motor_forward_pin, HIGH);
        digitalWrite(_motor_backward_pin, HIGH);
      }
    }
  }

private:
  uint8_t _motor_forward_pin, _motor_backward_pin, _close_end_pin, _open_end_pin;
  State _open_state = NONE;
};

#endif