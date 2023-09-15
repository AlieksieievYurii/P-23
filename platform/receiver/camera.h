#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <PWMServo.h> // Because the standart one has conflicts withe other libraries

PWMServo front_camera_servo;

void handle_front_camera_control(uint8_t con_value) {
  int val = map(con_value, 0, 255, 0, 179);
  //Serial.println(val);
  front_camera_servo.write(val);
}

#endif