#ifndef __LIGHTS_H__
#define __LIGHTS_H__

void handle_lights(uint8_t lights_states) {
  uint8_t back_lights_state = lights_states & 0x1;
  uint8_t front_lights_state = lights_states & (0x1 << 1);
  uint8_t long_lights_state = lights_states & (0x1 << 2);
  digitalWrite(LEFT_BACK_LIGHT_PIN, back_lights_state);
  digitalWrite(RIGHT_BACK_LIGHT_PIN, back_lights_state);

  digitalWrite(LEFT_FRONT_LIGHT_PIN, front_lights_state);
  digitalWrite(RIGHT_FRONT_LIGHT_PIN, front_lights_state);

  digitalWrite(LEFT_LONG_LIGHT_PIN, long_lights_state);
  digitalWrite(RIGHT_LONG_LIGHT_PIN, long_lights_state);
}

#endif