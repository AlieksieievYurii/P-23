#ifndef __ROTATION_CONTROL_H__
#define __ROTATION_CONTROL_H__


class TurretRotation {
public:
  TurretRotation(const uint8_t dir_pin, const uint8_t step_pin) {
    _dir_pin = dir_pin;
    _step_pin = step_pin;
    
  }

  void write(uint8_t value) {
    if (value >= 0x01 && value <= 0x7F) {
      _speed = map(value, 0x01, 0x7F, 3000, 1200);
      _right = true;
    } else if (value >= 0x80 && value <= 0xFE) {
      _speed = map(value, 0x80, 0xFE, 3000, 1200);
      _right = false;
    } else {
      _speed = 0;
    }
  }

  void tick() {
    if (_speed != 0) {
      digitalWrite(_dir_pin, _right);
      digitalWrite(TURRET_ROTATION_ENABLE_PIN, LOW);
      if (_current_speed != _speed) {
        Timer2.setPeriod(_speed);
        Timer2.enableISR(CHANNEL_A);
        Timer2.restart();
      }
    } else {
      Timer2.stop();
      digitalWrite(TURRET_ROTATION_ENABLE_PIN, HIGH);
    }
    _current_speed =_speed;
  }

  void interupt_call() {
    digitalWrite(_step_pin, !digitalRead(_step_pin));
  }

private:
  uint8_t _dir_pin, _step_pin;
  volatile uint16_t _speed = 0;
  volatile uint16_t _current_speed = 0;
  volatile bool _right = true;
};

#endif