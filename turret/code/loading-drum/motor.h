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

enum class State {
  NONE,
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

class Gate {
public:
  void open() {
    _state = State::OPEN;
  }

  void close() {
    _state = State::CLOSE;
  }

  void tick() {
    switch (_state) {

      case State::OPEN:
        if (GATE_OPENED_TRIGGERED) {
          STOP_GATE;
        } else {
          OPEN_GATE;
        }
        break;

      case State::CLOSE:
        if (GATE_CLOSED_TRIGGERED) {
          STOP_GATE;
        } else {
          CLOSE_GATE;
        }
        break;
    }
  }

private:
  State _state = State::NONE;
};

class LoadingDrumMover {
public:
  void activate() {
    _state = State::OPEN;
  }

  void deactivate() {
    _state = State::CLOSE;
  }

  void tick(void) {
    switch (_state) {

      case State::OPEN:
        if (digitalRead(LOADING_DRUM_MOVER_MOTOR_FRONT_END_PIN)) {
          MOVE_LOADING_DRUM_FORWARD;
        } else {
          STOP_LOADING_DRUM_MOVER;
        }
        break;

      case State::CLOSE:
        if (digitalRead(LOADING_DRUM_MOVER_MOTOR_BACK_END_PIN)) {
          MOVE_LOADING_DRUM_BACKWARD;
        } else {
          STOP_LOADING_DRUM_MOVER;
        }
        break;
    }
  }

private:
  State _state = State::NONE;
};

class DrumRotation {
public:
  void next() {
    _rotation_state = RotationState::NEXT;
    ROTATE_CLOCKWISE_DRUM;
    _timestamp = millis();
  }

  void previous() {
    _rotation_state = RotationState::PREVIOUS;
    ROTATE_COUNTER_CLOCKWISE_DRUM;
    _timestamp = millis();
  }

  void tick(void) {
    switch (_rotation_state) {

      case RotationState::NEXT:
        ROTATE_CLOCKWISE_DRUM;
        if (millis() - _timestamp > 100 && ROTATION_CHECKPOINT_TRIGGERED) {
          STOP_ROTATION;
          _rotation_state = RotationState::STOP;
        }
        break;
        
      case RotationState::PREVIOUS:
        ROTATE_COUNTER_CLOCKWISE_DRUM;
        if (millis() - _timestamp > 100 && ROTATION_CHECKPOINT_TRIGGERED) {
          STOP_ROTATION;
          _rotation_state = RotationState::STOP;
        }
        break;
    }
  }
private:
  RotationState _rotation_state = RotationState::STOP;
  uint32_t _timestamp = 0;
};

class Feeder {
  public:
    void tick(void) {
      switch(_state) {

        case FeederState::PUSH:
          PUSH_FEEDER;
          if (FEEDER_PUSHED_POSITION_TRIGGERED) {
            STOP_FEEDER;
            _state = FeederState::STOP;
          }
          break;

        case FeederState::PULL:
          PULL_FEEDER;
          if (millis() - _timestamp > 700 && FEEDER_PULLED_POSITION_TRIGGERED) {
            STOP_FEEDER;
            _state = FeederState::STOP;
          }
          break;
      }
    }

    void push() {
      _state = FeederState::PUSH;
      
    }

    void pull() {
      _state = FeederState::PULL;
      _timestamp = millis();
    }
  
  private:
    FeederState _state = FeederState::STOP;
    uint32_t _timestamp = 0;
};

#endif