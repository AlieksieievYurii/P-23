#include <Wire.h>

#include "config.h"
#include "motor.h"

#define DEBUG

Gate gate;
LoadingDrumMover loading_drum_mover;
DrumRotation drum_rotation;
Feeder feeder;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  init_pins();
  Wire.begin(0x01);
  Wire.onReceive(on_receive_command);
}

void on_receive_command(uint8_t bytes) {
  if (bytes == 1) {
    uint8_t command = Wire.read();
    Serial.println(command);
    execute_command(command);
  }
}

void execute_command(uint8_t command) {
  switch (command) {
    case CALIBRATION_COMMAND:
      loading_drum_mover.deactivate();
      gate.close();
      drum_rotation.next();
      break;
    case 2:
      loading_drum_mover.activate();
      gate.open();
      drum_rotation.previous();
      break;
  }
}
uint32_t a = 0;
void loop() {
#ifdef DEBUG
  if (Serial.available() > 0) {
    char buff[2] = { 0 };
    Serial.readString().toCharArray(buff, 2);
    uint8_t command = (uint8_t)strtol(buff, 0, 16);
    execute_command(command);
  }
#endif
  // if(millis() - a > 3000) {
  //   drum_rotation.next();
  //   a = millis();
  // }
  loading_drum_mover.tick();
  gate.tick();
  drum_rotation.tick();
  feeder.tick();
}
