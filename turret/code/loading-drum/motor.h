#ifndef __MOTOR_H__
#define __MOTOR_H__

#define OPEN_GATE \
  digitalWrite(GATE_MOTOR_FORWARD_PIN, LOW); \
  digitalWrite(GATE_MOTOR_BACKWARD_PIN, HIGH)

#define CLOSE_GATE \
  digitalWrite(GATE_MOTOR_FORWARD_PIN, HIGH); \
  digitalWrite(GATE_MOTOR_BACKWARD_PIN, LOW)

#define STOP_GATE \
  digitalWrite(GATE_MOTOR_FORWARD_PIN, HIGH); \
  digitalWrite(GATE_MOTOR_BACKWARD_PIN, HIGH)

#define GATE_CLOSED_TRIGGERED analogRead(GATE_CLOSED_END_PIN) > 500
#define GATE_OPENED_TRIGGERED analogRead(GATE_OPENED_END_PIN) > 500

#define MOVE_LOADING_DRUM_FORWARD \
  digitalWrite(LOADING_DRUM_MOVER_MOTOR_BACKWARD_PIN, LOW); \
  digitalWrite(LOADING_DRUM_MOVER_MOTOR_FORWARD_PIN, HIGH)

#define MOVE_LOADING_DRUM_BACKWARD \
  digitalWrite(LOADING_DRUM_MOVER_MOTOR_BACKWARD_PIN, HIGH); \
  digitalWrite(LOADING_DRUM_MOVER_MOTOR_FORWARD_PIN, LOW)

#define STOP_LOADING_DRUM_MOVER \
  digitalWrite(LOADING_DRUM_MOVER_MOTOR_BACKWARD_PIN, HIGH); \
  digitalWrite(LOADING_DRUM_MOVER_MOTOR_FORWARD_PIN, HIGH)

#define ROTATE_CLOCKWISE_DRUM \
  digitalWrite(LOADING_DRUM_ROTATION_MOTOR_FORWARD_PIN, HIGH); \
  digitalWrite(LOADING_DRUM_ROTATION_MOTOR_BACKWARD_PIN, LOW)

#define ROTATE_COUNTER_CLOCKWISE_DRUM \
  digitalWrite(LOADING_DRUM_ROTATION_MOTOR_FORWARD_PIN, LOW); \
  digitalWrite(LOADING_DRUM_ROTATION_MOTOR_BACKWARD_PIN, HIGH)

#define STOP_ROTATION \
  digitalWrite(LOADING_DRUM_ROTATION_MOTOR_FORWARD_PIN, HIGH); \
  digitalWrite(LOADING_DRUM_ROTATION_MOTOR_BACKWARD_PIN, HIGH)

#define ROTATION_CHECKPOINT_TRIGGERED analogRead(LOADING_DRUM_ROTATION_MOTOR_POSITION_PIN) < 500

#define PUSH_FEEDER \
  digitalWrite(FEEDER_MOTOR_FORWARD_PIN, HIGH); \
  digitalWrite(FEEDER_MOTOR_BACKWARD_PIN, LOW)

#define PULL_FEEDER \
  digitalWrite(FEEDER_MOTOR_FORWARD_PIN, LOW); \
  digitalWrite(FEEDER_MOTOR_BACKWARD_PIN, HIGH)

#define STOP_FEEDER \
  digitalWrite(FEEDER_MOTOR_FORWARD_PIN, HIGH); \
  digitalWrite(FEEDER_MOTOR_BACKWARD_PIN, HIGH)

#define FEEDER_PUSHED_POSITION_TRIGGERED analogRead(FEEDER_CONSTRAIN_IR_PIN) < 500

#define FEEDER_PULLED_POSITION_TRIGGERED digitalRead(FEEDER_CONSTRAIN_END_PIN)

#define ENABLE_LOADING_DRUM_MOVER_TIMEOUT
#define ENABLE_GATE_TIMEOUT
#define ENABLE_FEEDER_TIMEOUT
#define ENABLE_DRUM_ROTATION_TIMEOUT

#define LOADING_DRUM_MOVER_TIMEOUT 11000
#define GATE_TIMEOUT 1000
#define FEEDER_TIMEOUT 3000
#define DRUM_ROTATION_TIMEOUT 1000

enum class State {
  STOP,
  OPEN,
  CLOSE
};

enum class RotationState {
  NEXT,
  PREVIOUS,
  STOP
};

enum class FeederState {
  PUSH,
  PULL,
  STOP
};

enum class Status {
  DOING,
  FINISHED,
  FAILED
};

// Error codes
#define MOTOR_ERROR_GATE_OPENING_TIMEOUT 0x2
#define MOTOR_ERROR_GATE_CLOSING_TIMEOUT 0x3

#define MOTOR_ERROR_LOADING_DRUM_ACTIVATION_TIMEOUT 0x4
#define MOTOR_ERROR_LOADING_DRUM_DEACTIVATION_TIMEOUT 0x5

#define MOTOR_ERROR_FEEDER_PUSH_TIMEOUT 0x6
#define MOTOR_ERROR_FEEDER_PULL_TIMEOUT 0x7


class Gate {
public:
  void open() {
    _state = State::OPEN;
    _status = Status::DOING;
    _error_code = 0;
    _timestamp = millis();
  }

  void close() {
    _state = State::CLOSE;
    _status = Status::DOING;
    _error_code = 0;
    _timestamp = millis();
  }

  Status get_status(void) {
    return _status;
  }

  uint8_t get_error_code(void) {
    if (_status != Status::FAILED)
      return 0;
    else
      return _error_code;
  }

  void tick() {
    if (_status == Status::FAILED) {
      _state = State::STOP;
    }

    switch (_state) {
      case State::OPEN:
        if (GATE_OPENED_TRIGGERED) {
          _state = State::STOP;
          _status = Status::FINISHED;
        } else {
#ifdef ENABLE_GATE_TIMEOUT
          if (millis() - _timestamp > GATE_TIMEOUT) {
            _status = Status::FAILED;
            _error_code = MOTOR_ERROR_GATE_OPENING_TIMEOUT;
          }
#endif
          OPEN_GATE;
        }
        break;

      case State::CLOSE:
        if (GATE_CLOSED_TRIGGERED) {
          _state = State::STOP;
          _status = Status::FINISHED;
        } else {
#ifdef ENABLE_GATE_TIMEOUT
          if (millis() - _timestamp > GATE_TIMEOUT) {
            _status = Status::FAILED;
            _error_code = MOTOR_ERROR_GATE_CLOSING_TIMEOUT;
          }
#endif
          CLOSE_GATE;
        }
        break;

      case State::STOP:
        STOP_GATE;
        break;
    }
  }

private:
  State _state = State::STOP;
  Status _status = Status::FINISHED;
  uint32_t _timestamp = 0;
  uint8_t _error_code = 0;
};

class LoadingDrumMover {
public:
  void activate() {
    _state = State::OPEN;
    _status = Status::DOING;
    _error_code = 0;
    _timestamp = millis();
  }

  void deactivate() {
    _state = State::CLOSE;
    _status = Status::DOING;
    _error_code = 0;
    _timestamp = millis();
  }

  Status get_status() {
    return _status;
  }

  uint8_t get_error_code(void) {
    if (_status != Status::FAILED)
      return 0;
    else
      return _error_code;
  }

  void tick(void) {
    if (_status == Status::FAILED) {
      _state = State::STOP;
    }

    switch (_state) {
      case State::OPEN:
        if (digitalRead(LOADING_DRUM_MOVER_MOTOR_FRONT_END_PIN)) {

#ifdef ENABLE_LOADING_DRUM_MOVER_TIMEOUT
          if (millis() - _timestamp > LOADING_DRUM_MOVER_TIMEOUT) {
            _status = Status::FAILED;
            _error_code = MOTOR_ERROR_LOADING_DRUM_ACTIVATION_TIMEOUT;
          }
#endif

          MOVE_LOADING_DRUM_FORWARD;

        } else {
          _state = State::STOP;
          _status = Status::FINISHED;
        }
        break;

      case State::CLOSE:
        if (digitalRead(LOADING_DRUM_MOVER_MOTOR_BACK_END_PIN)) {

#ifdef ENABLE_LOADING_DRUM_MOVER_TIMEOUT
          if (millis() - _timestamp > LOADING_DRUM_MOVER_TIMEOUT) {
            _status = Status::FAILED;
            _error_code = MOTOR_ERROR_LOADING_DRUM_DEACTIVATION_TIMEOUT;
          }
#endif

          MOVE_LOADING_DRUM_BACKWARD;

        } else {
          _state = State::STOP;
          _status = Status::FINISHED;
        }
        break;

      case State::STOP:
        STOP_LOADING_DRUM_MOVER;
        break;
    }
  }

private:
  State _state = State::STOP;
  Status _status = Status::FINISHED;
  uint32_t _timestamp = 0;
  uint8_t _error_code = 0;
};

class DrumRotation {
public:
  void next() {
    _rotation_state = RotationState::NEXT;
    _status = Status::DOING;
    _error_code = 0;
    _timestamp = millis();
  }

  void previous() {
    _rotation_state = RotationState::PREVIOUS;
    _status = Status::DOING;
    _error_code = 0;
    _timestamp = millis();
  }

  Status get_status(void) {
    return _status;
  }

  uint8_t get_error_code(void) {
    if (_status != Status::FAILED)
      return 0;
    else
      return _error_code;
  }

  void tick(void) {
    switch (_rotation_state) {

      case RotationState::NEXT:
        ROTATE_CLOCKWISE_DRUM;
#ifdef ENABLE_DRUM_ROTATION_TIMEOUT
        if (millis() - _timestamp >= DRUM_ROTATION_TIMEOUT) {
          STOP_ROTATION;
          _rotation_state = RotationState::STOP;
          _status = Status::FAILED;
          _error_code = ERROR_DRUM_CLOCKWISE_ROTATION_TIMEOUT;
          return;
        }
#endif
        if (millis() - _timestamp > 100 && ROTATION_CHECKPOINT_TRIGGERED) {
          STOP_ROTATION;
          _status = Status::FINISHED;
          _rotation_state = RotationState::STOP;
        }
        break;

      case RotationState::PREVIOUS:
        ROTATE_COUNTER_CLOCKWISE_DRUM;
#ifdef ENABLE_DRUM_ROTATION_TIMEOUT
        if (millis() - _timestamp >= DRUM_ROTATION_TIMEOUT) {
          STOP_ROTATION;
          _rotation_state = RotationState::STOP;
          _status = Status::FAILED;
          _error_code = ERROR_DRUM_COUNTER_CLOCKWISE_ROTATION_TIMEOUT;
          return;
        }
#endif
        if (millis() - _timestamp > 100 && ROTATION_CHECKPOINT_TRIGGERED) {
          STOP_ROTATION;
          _status = Status::FINISHED;
          _rotation_state = RotationState::STOP;
        }
        break;
    }
  }
private:
  RotationState _rotation_state = RotationState::STOP;
  Status _status = Status::FINISHED;
  uint32_t _timestamp = 0;
  uint8_t _error_code = 0;
};

class Feeder {
public:
  void push() {
    _state = FeederState::PUSH;
    _timestamp = millis();
  }

  void pull() {
    _state = FeederState::PULL;
    _timestamp = millis();
  }

  Status get_status(void) {
    return _status;
  }

  uint8_t get_error_code(void) {
    if (_status != Status::FAILED)
      return 0;
    else
      return _error_code;
  }

  void tick(void) {
    switch (_state) {

      case FeederState::PUSH:
        _status = Status::DOING;
        PUSH_FEEDER;
#ifdef ENABLE_FEEDER_TIMEOUT
        if (millis() - _timestamp >= FEEDER_TIMEOUT) {
          _status = Status::FAILED;
          _error_code = MOTOR_ERROR_FEEDER_PUSH_TIMEOUT;
          _state = FeederState::STOP;
          return;
        }
#endif
        if (FEEDER_PUSHED_POSITION_TRIGGERED) {
          _status = Status::FINISHED;
          _state = FeederState::STOP;
        }
        break;

      case FeederState::PULL:
        _status = Status::DOING;
        PULL_FEEDER;
#ifdef ENABLE_FEEDER_TIMEOUT
        if (millis() - _timestamp >= FEEDER_TIMEOUT) {
          _status = Status::FAILED;
          _error_code = MOTOR_ERROR_FEEDER_PULL_TIMEOUT;
          _state = FeederState::STOP;
          return;
        }
#endif
        if (millis() - _timestamp > 700 && FEEDER_PULLED_POSITION_TRIGGERED) {
          _status = Status::FINISHED;
          _state = FeederState::STOP;
        }
        break;

      case FeederState::STOP:
        STOP_FEEDER;
        break;
    }
  }

private:
  FeederState _state = FeederState::STOP;
  Status _status = Status::FINISHED;
  uint32_t _timestamp = 0;
  uint8_t _error_code = 0;
};

#endif