#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <PWMServo.h>  // Because the standart one has conflicts withe other libraries
#include "osd.h"

#define MIN(VALUE, CONSTRAIN) (((VALUE) > (CONSTRAIN)) ? (VALUE) : (CONSTRAIN))
#define MAX(VALUE, CONSTRAIN) (((VALUE) > (CONSTRAIN)) ? (CONSTRAIN) : (VALUE))

#define CAMERA_SWITCH_UPDATE_TIME 500

PWMServo front_camera_servo;
Osd osd(Serial3);

#define FRONT_CAMERA_ID 0x0
#define BACK_CAMERA_ID 0x1
#define TURRET_CAMERA_ID 0x2
#define GUN_CAMERA_ID 0x3
#define A_CAMERA_ID 0x4
#define R_CAMERA_ID 0x5

volatile uint8_t current_selected_camera = FRONT_CAMERA_ID;

void handle_front_camera_control(uint8_t con_value, bool camera_mode_bind, uint8_t speed_camera_movement) {
  static uint8_t val;

  if (camera_mode_bind) {
    val = map(con_value, 0, 255, 0, 179);
  } else {
    uint8_t k = map(speed_camera_movement, 0, 255, 0, 10);
    if (con_value <= 120)
      val = MIN(1, val - k);
    else if (con_value >= 136)
      val = MAX(179, val + k);
  }
  front_camera_servo.write(val);
}

uint8_t handle_camera_selection(uint8_t value) {
  static uint32_t time = 0;
  static bool is_camera_set = false;
  uint8_t selected_camera = value & 0x7;

  if (selected_camera != current_selected_camera) {
    time = millis();
    is_camera_set = false;
    current_selected_camera = selected_camera;
  }

  if (is_camera_set == false && millis() - time >= CAMERA_SWITCH_UPDATE_TIME) {
    digitalWrite(FRONT_CAMERA_SWITCH_PIN, selected_camera == FRONT_CAMERA_ID);
    digitalWrite(BACK_CAMERA_SWITCH_PIN, selected_camera == BACK_CAMERA_ID);
    digitalWrite(TURRET_CAMERA_SWITCH_PIN, selected_camera == TURRET_CAMERA_ID);
    digitalWrite(GUN_CAMERA_SWITCH_PIN, selected_camera == GUN_CAMERA_ID);

    digitalWrite(A_CAMERA_SWITCH_PIN, selected_camera == A_CAMERA_ID);
    digitalWrite(R_CAMERA_SWITCH_PIN, selected_camera == R_CAMERA_ID);
    is_camera_set = true;
  }

  return selected_camera;
}

#endif