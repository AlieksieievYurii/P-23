#ifndef __GATE_H__
#define __GATE_H__

#define IS_END_TRIGGERED !digitalRead(BREECH_BLOCK_GATE_CONSTRAIN_END_PIN)
#define IS_IR_CUT digitalRead(BREECH_BLOCK_GATE_IR_PIN)
#define OPEN_GATE \
  digitalWrite(BREECH_BLOCK_GATE_MOTOR_OPENING_PIN, HIGH); \
  digitalWrite(BREECH_BLOCK_GATE_MOTOR_CLOSING_PIN, LOW);
#define CLOSE_GATE \
  digitalWrite(BREECH_BLOCK_GATE_MOTOR_OPENING_PIN, LOW); \
  digitalWrite(BREECH_BLOCK_GATE_MOTOR_CLOSING_PIN, HIGH);
#define STOP_GATE \
  digitalWrite(BREECH_BLOCK_GATE_MOTOR_OPENING_PIN, HIGH); \
  digitalWrite(BREECH_BLOCK_GATE_MOTOR_CLOSING_PIN, HIGH);

enum State {
  NONE,
  OPEN,
  HOLD,
  CLOSE,
};

class Gate {
public:
  void open() {
    _state = OPEN;
  }

  void close() {
    if (_state == OPEN) {
      _ignore_hold_check = true;
    }
    _state = CLOSE;
  }

  bool hold() {
    if (_state == CLOSE)
      return false;

    _state = HOLD;
    
    return true;
  }

  void tick() {
    if (_state == OPEN) {

      if (!IS_END_TRIGGERED) {
        OPEN_GATE;
      } else {
        STOP_GATE;
      }

    } else if (_state == CLOSE) {

      if (_ignore_hold_check) {
        CLOSE_GATE;
        if (IS_IR_CUT) {
          _ignore_hold_check = false;
        }
      } else if (IS_IR_CUT) {
        CLOSE_GATE;
      } else {
        STOP_GATE;
      }

    } else if (_state == HOLD) {

      if (!IS_IR_CUT) {
        CLOSE_GATE;
      } else {
        STOP_GATE;
      }
    }
  }
private:
  State _state = NONE;
  bool _ignore_hold_check = false;
};

#endif