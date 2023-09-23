#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include "config.h"

#define LIGHT_MODE_STANDALONE 0x0
#define LIGHT_MODE_BLINK 0x1
#define LIGHT_MODE_BLINK_FAST 0x2

#define BLICK_FREQ 500
#define BLICK_FAST_FREQ 50

class Lights {
public:
  uint8_t back_light_mode = LIGHT_MODE_STANDALONE;
  uint8_t front_light_mode = LIGHT_MODE_STANDALONE;
  uint8_t long_light_mode = LIGHT_MODE_STANDALONE;
  uint8_t turret_light_mode = LIGHT_MODE_STANDALONE;

  bool black_light = false;
  bool front_light = false;
  bool long_light = false;
  bool turret_light = false;



  void tick() {

    if (back_light_mode == LIGHT_MODE_STANDALONE) {
      digitalWrite(LEFT_BACK_LIGHT_PIN, black_light);
      digitalWrite(RIGHT_BACK_LIGHT_PIN, black_light);
    }

    if (front_light_mode == LIGHT_MODE_STANDALONE) {
      digitalWrite(LEFT_FRONT_LIGHT_PIN, front_light);
      digitalWrite(RIGHT_FRONT_LIGHT_PIN, front_light);
    }

    if (long_light_mode == LIGHT_MODE_STANDALONE) {
      digitalWrite(LEFT_LONG_LIGHT_PIN, long_light);
      digitalWrite(RIGHT_LONG_LIGHT_PIN, long_light);
    }

    // TODO Implement Turret lights

    if (millis() - _last_update > BLICK_FREQ) {

      if (back_light_mode == LIGHT_MODE_BLINK) {
        if (black_light)
          _black_light_state = !_black_light_state;
        else
          _black_light_state = false;

        digitalWrite(LEFT_BACK_LIGHT_PIN, _black_light_state);
        digitalWrite(RIGHT_BACK_LIGHT_PIN, _black_light_state);
      }

      if (front_light_mode == LIGHT_MODE_BLINK) {
        if (front_light)
          _front_light_state = !_front_light_state;
        else
          _front_light_state = false;
        digitalWrite(LEFT_FRONT_LIGHT_PIN, _front_light_state);
        digitalWrite(RIGHT_FRONT_LIGHT_PIN, _front_light_state);
      }

      if (long_light_mode == LIGHT_MODE_BLINK) {

        if (long_light)
          _long_light_state = !_long_light_state;
        else
          _long_light_state = false;
        digitalWrite(LEFT_LONG_LIGHT_PIN, _long_light_state);
        digitalWrite(RIGHT_LONG_LIGHT_PIN, _long_light_state);
      }

      _last_update = millis();
    }

    if (millis() - _last_update_fast > BLICK_FAST_FREQ) {

      if (back_light_mode == LIGHT_MODE_BLINK_FAST) {
        if (black_light)
          _black_light_state = !_black_light_state;
        else
          _black_light_state = false;
        digitalWrite(LEFT_BACK_LIGHT_PIN, _black_light_state);
        digitalWrite(RIGHT_BACK_LIGHT_PIN, _black_light_state);
      }

      if (front_light_mode == LIGHT_MODE_BLINK_FAST) {
        if (front_light)
          _front_light_state = !_front_light_state;
        else
          _front_light_state = false;
        digitalWrite(LEFT_FRONT_LIGHT_PIN, _front_light_state);
        digitalWrite(RIGHT_FRONT_LIGHT_PIN, _front_light_state);
      }

      if (long_light_mode == LIGHT_MODE_BLINK_FAST) {
        if (long_light)
          _long_light_state = !_long_light_state;
        else
          _long_light_state = false;
        digitalWrite(LEFT_LONG_LIGHT_PIN, _long_light_state);
        digitalWrite(RIGHT_LONG_LIGHT_PIN, _long_light_state);
      }

      _last_update_fast = millis();
    }
  }

private:
  uint32_t _last_update = 0;
  uint32_t _last_update_fast = 0;

  bool _black_light_state = false;
  bool _front_light_state = false;
  bool _long_light_state = false;
};

Lights lights;

void handle_lights(char* data) {
  uint8_t lights_states = data[0xC];
  uint8_t lights_modes = data[0xD];

  lights.back_light_mode = lights_modes & 0x3;
  lights.front_light_mode = lights_modes >> 2 & 0x3;
  lights.long_light_mode = lights_modes >> 4 & 0x3;
  lights.turret_light_mode = lights_modes >> 6 & 0x3;

  lights.black_light = lights_states & 0x1;
  lights.front_light = lights_states >> 1 & 0x1;
  lights.long_light = lights_states >> 2 & 0x1;
  lights.turret_light = lights_states >> 3 & 0x1;
}

#endif