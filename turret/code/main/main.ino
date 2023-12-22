#include "config.h"
#include "servo-control.h"
#include <GyverTimers.h>
#include "rotation-control.h"
#include "module.h"


Servo gun_vertical_positioning_servo;
Servo commander_camera_horizontal_servo;
Servo commander_camera_vertical_servo;
GunAngle gun_vertical_positioning_servo_control(gun_vertical_positioning_servo);
TurretRotation turret_horizontal_rotation(TURRET_ROTATION_DIR_PIN, TURRET_ROTATION_STEP_PIN);
CommanderServo commander_camera_horizontal_control(commander_camera_horizontal_servo);
CommanderServo commander_camera_vertical_control(commander_camera_vertical_servo);
Module module;

#define MESSAGE_SIZE 5  // Note it does not include Start-of-Frame delimiter and check-sum

ISR(TIMER2_A) {
  turret_horizontal_rotation.interupt_call();
}

void setup() {
  Serial.begin(9600);
  init_pins();
  gun_vertical_positioning_servo.attach(GUN_VERTICAL_SERVO_PIN);
  commander_camera_horizontal_servo.attach(COMMANDER_CAMERA_HORIZONTAL_SERVO_PIN);
  commander_camera_vertical_servo.attach(COMMANDER_CAMERA_VERTICAL_SERVO_PIN);
}

const uint8_t get_check_sum(const uint8_t* data) {
  uint16_t sum = 0;
  for (uint8_t i = 1; i <= MESSAGE_SIZE; i++)
    sum += data[i];
  return sum / MESSAGE_SIZE;
}

bool read_serial_message(uint8_t* data) {
  if (Serial.available() == 0)
    return false;

  uint32_t last_update_time = millis();
  uint8_t index = 1;
  bool start = false;

  while (true) {
    if (millis() - last_update_time > 1000)
      return false;

    if (Serial.available() == 0)
      continue;

    uint8_t incoming_byte = Serial.read();

    if (start) {

      data[index++] = incoming_byte;
      if (index == MESSAGE_SIZE + 2) {

        // Check the checksum which is the latest item
        if (data[MESSAGE_SIZE + 1] == get_check_sum(data)) {
          return true;
        } else {
          index = 1;
          start = false;
        }
      }
    }

    if (start == false && incoming_byte == 0xFF) {
      start = true;
      data[0] = 0xFF;
    }
  }
}


void loop() {
  uint8_t incoming_data[MESSAGE_SIZE + 2] = { 0 };

  if (read_serial_message(incoming_data)) {
    turret_horizontal_rotation.write(incoming_data[1]);
    gun_vertical_positioning_servo_control.write(incoming_data[2]);

    bool comander_camera_bind_mode = incoming_data[5] & 0x1;

    if (comander_camera_bind_mode)
      commander_camera_horizontal_control.set(incoming_data[3]);
    else
      commander_camera_horizontal_control.write(incoming_data[3]);

    if (comander_camera_bind_mode)
      commander_camera_vertical_control.set(incoming_data[4]);
    else
      commander_camera_vertical_control.write(incoming_data[4]);
  }

  gun_vertical_positioning_servo_control.tick();
  turret_horizontal_rotation.tick();
  commander_camera_horizontal_control.tick();
  commander_camera_vertical_control.tick();
  module.tick();
}
