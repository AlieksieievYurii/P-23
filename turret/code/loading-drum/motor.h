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

enum State {
  NONE,
  OPEN,
  CLOSE
};

class Gate {
public:
  void open() {
    _state = OPEN;
  }

  void close() {
    _state = CLOSE;
  }

  void tick() {
    switch (_state) {

      case OPEN:
        if (GATE_OPENED_TRIGGERED) {
          STOP_GATE;
        } else {
          OPEN_GATE;
        }
        break;

      case CLOSE:
        if (GATE_CLOSED_TRIGGERED) {
          STOP_GATE;
        } else {
          CLOSE_GATE;
        }
        break;
    }
  }

private:
  State _state = NONE;
};

class LoadingDrumMover {
public:
  void activate() {
    _state = OPEN;
  }

  void deactivate() {
    _state = CLOSE;
  }

  void tick(void) {
    switch (_state) {

      case OPEN:
        if (digitalRead(LOADING_DRUM_MOVER_MOTOR_FRONT_END_PIN)) {
          MOVE_LOADING_DRUM_FORWARD;
        } else {
          STOP_LOADING_DRUM_MOVER;
        }
        break;

      case CLOSE:
        if (digitalRead(LOADING_DRUM_MOVER_MOTOR_BACK_END_PIN)) {
          MOVE_LOADING_DRUM_BACKWARD;
        } else {
          STOP_LOADING_DRUM_MOVER;
        }
        break;
    }
  }

private:
  State _state = NONE;
};

#endif