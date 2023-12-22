#ifndef __MODULE_H__
#define __MODULE_H__

#include <Wire.h>

#define LOADER_DRUM_MODULE_ADDRESS 0x01

#define LOADING_DRUM_CALIBRATION_COMMAND 0x01

class Module {
public:
  Module() {
    Wire.begin();
  }

  void test() {
    static uint8_t x = 0;
    Wire.beginTransmission(LOADER_DRUM_MODULE_ADDRESS); // transmit to device #8
    Wire.write(2);              // sends one byte
    Wire.endTransmission();
  }

  void loading_drum_calibrate() {
    Wire.beginTransmission(LOADER_DRUM_MODULE_ADDRESS);
    Wire.write(LOADING_DRUM_CALIBRATION_COMMAND);
    Wire.endTransmission();
  }

  void tick() {

  }
};

#endif