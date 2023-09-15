#ifndef __DRIVING_H__
#define __DRIVING_H__

#define MIN(VALUE, CONSTRAIN) (((VALUE) > (CONSTRAIN)) ? (VALUE) : (CONSTRAIN))
#define MAX(VALUE, CONSTRAIN) (((VALUE) > (CONSTRAIN)) ? (CONSTRAIN) : (VALUE))

#define DISABLE_TRACKS() set_track_motors_state(false)
#define ENABLE_TRACKS() set_track_motors_state(true)

#define POWER_CHANGE_STEP 5

void set_left_track(uint8_t power, bool forward) {
  if (forward) {
    analogWrite(LEFT_MOTOR_DRIVE_LPWM, power);
    analogWrite(LEFT_MOTOR_DRIVE_RPWM, 0);
  } else {
    analogWrite(LEFT_MOTOR_DRIVE_RPWM, power);
    analogWrite(LEFT_MOTOR_DRIVE_LPWM, 0);
  }
}

void set_right_track(uint8_t power, bool forward) {
  if (forward) {
    analogWrite(RIGHT_MOTOR_DRIVE_LPWM, power);
    analogWrite(RIGHT_MOTOR_DRIVE_RPWM, 0);
  } else {
    analogWrite(RIGHT_MOTOR_DRIVE_RPWM, power);
    analogWrite(RIGHT_MOTOR_DRIVE_LPWM, 0);
  }
}

/*
* Smoothly changes the direction and speed for left track to avoid over-loading the motor
*/
void set_smoothly_left_track(uint8_t track_power, bool forward) {
  static uint8_t latest_left_track_power = 0;
  static bool latest_left_forward = true;

  if (forward != latest_left_forward) {
    if (latest_left_track_power == 0)
      latest_left_forward = forward;
    else
      latest_left_track_power = MIN(latest_left_track_power - POWER_CHANGE_STEP, 0);
  } else {
    if (track_power > latest_left_track_power)
      latest_left_track_power = MAX(latest_left_track_power + POWER_CHANGE_STEP, track_power);
    else if (track_power < latest_left_track_power)
      latest_left_track_power = MIN(latest_left_track_power - POWER_CHANGE_STEP, 0);
  }

  set_left_track(latest_left_track_power, latest_left_forward);
}

/*
* Smoothly changes the direction and speed for right track to avoid over-loading the motor
*/
void set_smoothly_right_track(uint8_t track_power, bool forward) {
  static uint8_t latest_right_track_power = 0;
  static bool latest_right_forward = true;

  if (forward != latest_right_forward) {
    if (latest_right_track_power == 0)
      latest_right_forward = forward;
    else
      latest_right_track_power = MIN(latest_right_track_power - POWER_CHANGE_STEP, 0);
  } else {
    if (track_power > latest_right_track_power)
      latest_right_track_power = MAX(latest_right_track_power + POWER_CHANGE_STEP, track_power);
    else if (track_power < latest_right_track_power)
      latest_right_track_power = MIN(latest_right_track_power - POWER_CHANGE_STEP, 0);
  }

  set_right_track(latest_right_track_power, latest_right_forward);
}

/*
* Handles the main logic of movement of the tank. Depending on the joysticks values X,Y,R - moves the tank corespondenlty.
* Note: If the tank is driving forward or backward -> 180-degree rotation is disabled by the jostric R.
* Parameter 'speed' - sets liear speed.
* Parameter 'smoothly'(bool) - if sets True, then the speed and direction will change smoothly (linear) to avoid overloading the motors 
*/
void handle_driving(uint8_t joy_x, uint8_t joy_y, uint8_t joy_r, uint8_t speed, bool smoothly) {
  uint8_t left_track_power = 0;
  uint8_t right_track_power = 0;
  bool left_track_forward = true;
  bool right_track_forward = true;
  bool block_rotation = false;

  if (joy_y >= 150) {
    uint8_t value = map(joy_y, 150, 255, 0, speed);
    left_track_forward = true;
    right_track_forward = true;
    left_track_power = value;
    right_track_power = value;
    block_rotation = true;
  } else if (joy_y <= 120) {
    uint8_t value = map(joy_y, 120, 0, 0, speed);
    left_track_forward = false;
    right_track_forward = false;
    left_track_power = value;
    right_track_power = value;
    block_rotation = true;
  }

  if (joy_x >= 140) {
    uint8_t value = map(joy_x, 140, 255, 0, speed);
    right_track_power = MAX(right_track_power + value, speed);
    left_track_power = MIN(left_track_power - value, 0);
    block_rotation = true;
  } else if (joy_x <= 115) {
    uint8_t value = map(joy_x, 115, 0, 0, speed);
    right_track_power = MIN(right_track_power - value, 0);
    left_track_power = MAX(left_track_power + value, speed);
    block_rotation = true;
  }

  if (!block_rotation && joy_r >= 150) {
    uint8_t value = map(joy_r, 150, 255, 0, speed);
    left_track_forward = true;
    right_track_forward = false;
    left_track_power = value;
    right_track_power = value;
  } else if (!block_rotation && joy_r <= 115) {
    uint8_t value = map(joy_r, 115, 0, 0, speed);
    left_track_forward = false;
    right_track_forward = true;
    left_track_power = value;
    right_track_power = value;
  }

  if (smoothly) {
    set_smoothly_left_track(left_track_power, left_track_forward);
    set_smoothly_right_track(right_track_power, right_track_forward);
  } else {
    set_left_track(left_track_power, left_track_forward);
    set_right_track(right_track_power, right_track_forward);
  }
}

void set_track_motors_state(bool is_enabled) {
  digitalWrite(RIGHT_MOTOR_DRIVE_R_EN, is_enabled);
  digitalWrite(LEFT_MOTOR_DRIVE_R_EN, is_enabled);
  digitalWrite(RIGHT_MOTOR_DRIVE_L_EN, is_enabled);
  digitalWrite(LEFT_MOTOR_DRIVE_L_EN, is_enabled);
}

#endif