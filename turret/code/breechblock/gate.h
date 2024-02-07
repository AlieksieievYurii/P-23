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

#define ENABLE_GATE_TIMEOUT
#define GATE_TIMEOUT 9000

enum State {
  NONE,
  OPEN,
  HOLD,
  CLOSE,
};

class Gate {
public:
  void (*callback)(const bool, const uint8_t) = NULL;

  void open() {
    _set_state(OPEN);
    _timestamp = millis();
  }

  void close() {
    if (_state == OPEN) {
      _ignore_hold_check = true;
    }
    _set_state(CLOSE);
    _timestamp = millis();
  }

  hold() {
    if (_state == CLOSE) {
      callback(false, ERROR_NOT_ALLOWED_TO_PRE_CLOSE_GATE_FROM_CLOSED_STATE);
      return;
    }
    _set_state(HOLD);
    _timestamp = millis();
  }

  void tick() {
    if (_state_ritched)
      return;

#ifdef ENABLE_GATE_TIMEOUT
    if (millis() - _timestamp >= GATE_TIMEOUT) {
      STOP_GATE;
      _send_status(false, ERROR_GATE_TIMEOUT);
      return;
    }
#endif

    if (_state == OPEN) {

      if (!IS_END_TRIGGERED) {
        OPEN_GATE;
      } else {
        STOP_GATE;
        _send_status(true, NO_ERROR);
      }

    } else if (_state == CLOSE) {
      if (_ignore_hold_check) {
        CLOSE_GATE;

        static uint32_t timer = 0;
        if (IS_IR_CUT) {

          if (timer == 0)
            timer = millis();

          if (millis() - timer > 300) {
            _ignore_hold_check = false;
            timer = 0;
          }
        }
      } else if (IS_IR_CUT) {
        CLOSE_GATE;
      } else {
        STOP_GATE;
        _send_status(true, NO_ERROR);
      }

    } else if (_state == HOLD) {

      if (!IS_IR_CUT) {
        CLOSE_GATE;
      } else {
        STOP_GATE;
        _send_status(true, NO_ERROR);
      }
    }
  }
private:
  State _state = NONE;
  bool _ignore_hold_check = false;
  bool _state_ritched = true;
  uint32_t _timestamp = 0;

  void _send_status(const bool successful, const uint8_t error_code) {
    _state_ritched = true;
    callback(successful, error_code);
  }

  void _set_state(State state) {
    _state = state;
    _state_ritched = false;
  }
};

#endif