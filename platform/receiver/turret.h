#ifndef __TURRET_H__
#define __TURRET_H__

#include <Arduino.h>

#include "command_engine.h"

#define OUTCOMING_MESSAGE_SIZE 6
#define INCOMING_MESSAGE_SIZE 4

_CommandEngineClient CEModule;

enum class BattleMode {
  SAFE,
  ARCADE,
  STATIC
};

enum class LightsMode {
  CONSTANT,
  BLINKING,
  FAST_BLINKING
};

const uint8_t get_check_sum(const uint8_t* data, const uint8_t message_size) {
  uint16_t sum = 0;
  for (uint8_t i = 1; i <= message_size; i++)
    sum += data[i];
  return sum / message_size;
}

class Turret {
public:
  bool gun_camera_selected = false;
  bool comander_camera_selected = false;
  bool comander_camera_bind_mode = false;
  bool shell_connected = false;

  Turret(HardwareSerial& serial_stream) {
    _serial_stream = &serial_stream;
  }

  /*
  * Turret rotation. Value must be in the range 0..255.
  * If the value is from 1..127 -> the turret turns left
  * If the value is from 128..255 -> the turret turns right
  */
  void set_horizontal_position(uint8_t value, uint8_t speed) {
    if (value >= 0 && value <= 120) {
      _horizontal_position = map(value, 120, 0, 0x0, map(speed, 0, 255, 0x0, 0x7F));
    } else if (value >= 135 && value <= 255) {
      _horizontal_position = map(value, 135, 255, 0x80, map(speed, 0, 255, 0x80, 0xFE));
    } else {
      _horizontal_position = 0;
    }
  }

  /* Gun angle. Value must be in the range 0..255.
  * If the value is from 1..127 -> the gun goes down.
  * If the value is from 128..255 -> the gun goes up
  */
  void set_vertical_position(uint8_t value, uint8_t speed) {
    if (value >= 0 && value <= 120) {
      _vertical_position = map(value, 120, 0, 0x0, map(speed, 0, 255, 0x0, 0x7F));
    } else if (value >= 135 && value <= 255) {
      _vertical_position = map(value, 135, 255, 0x80, map(speed, 0, 255, 0x80, 0xFE));
    } else {
      _vertical_position = 0;
    }
  }

  void set_comander_camera_vertical_position(uint8_t value, uint8_t speed) {
    if (comander_camera_bind_mode) {
      _comander_camera_vertical_position = value;
      return;
    }

    if (value >= 0 && value <= 120) {
      _comander_camera_vertical_position = map(value, 120, 0, 0x80, map(speed, 0, 255, 0x80, 0xFE));
    } else if (value >= 130 && value <= 255) {
      _comander_camera_vertical_position = map(value, 130, 255, 0x0, map(speed, 0, 255, 0x0, 0x7F));
    } else {
      _comander_camera_vertical_position = 0;
    }
  }

  void set_comander_camera_horizontal_position(uint8_t value, uint8_t speed) {
    if (comander_camera_bind_mode) {
      _comander_camera_horizontal_position = value;
      return;
    }

    if (value >= 0 && value <= 120) {
      _comander_camera_horizontal_position = map(value, 120, 0, 0x0, map(speed, 0, 255, 0x80, 0xFE));
    } else if (value >= 130 && value <= 255) {
      _comander_camera_horizontal_position = map(value, 130, 255, 0x80, map(speed, 0, 255, 0x0, 0x7F));
    } else {
      _comander_camera_horizontal_position = 0;
    }
  }

  void set_lights(const uint8_t value, const uint8_t lights_mode) {
    _turret_lights = value >> 3 & 0x1;
    switch (lights_mode >> 6 & 0x3) {
      case 0:
        _turret_lights_mode = LightsMode::CONSTANT;
        break;
      case 1:
        _turret_lights_mode = LightsMode::BLINKING;
        break;
      case 2:
        _turret_lights_mode = LightsMode::FAST_BLINKING;
        break;
    }
  }

   void set_laser(const uint8_t value) {
    _laser = (value >> 4) & 0x1;
  }

  void set_warning_led(const uint8_t value) {
    _warning_led = (value >> 6) & 0x1;
  }

  void set_battle_mode(uint8_t value) {
    switch ((value >> 2) & 3) {
      case 0:
        _battle_mode = BattleMode::SAFE;
        break;
      case 1:
        _battle_mode = BattleMode::ARCADE;
        break;
      case 2:
        _battle_mode = BattleMode::STATIC;
        break;
    }
  }

  const uint8_t get_loader_state() {
    return _loader_state;
  }

  void tick() {
    uint8_t incoming_data[INCOMING_MESSAGE_SIZE + 2] = { 0 };
    _send_data();
    static uint32_t l = 0;
    
    if (millis() - l >= 500) {
      if (_read_serial_message(incoming_data)) {
        Serial.println("REVE");
        CEModule.set_incoming_data(incoming_data[0x1], incoming_data[0x2], incoming_data[0x4]);
        shell_connected = (incoming_data[0x3] >> 3) & 0x1;
        _loader_state = incoming_data[0x3] & 0x7;
      }
      l = millis();
    }
  }

private:
  BattleMode _battle_mode = BattleMode::SAFE;
  HardwareSerial* _serial_stream;
  volatile uint8_t _horizontal_position = 0;
  volatile uint8_t _vertical_position = 0;
  volatile uint8_t _comander_camera_vertical_position = 0;
  volatile uint8_t _comander_camera_horizontal_position = 0;
  volatile uint8_t _loader_state = 0;
  volatile bool _laser = false;
  volatile bool _warning_led = false;
  volatile bool _turret_lights = false;
  volatile LightsMode _turret_lights_mode = LightsMode::CONSTANT;

  void _send_data() {
    uint8_t data[OUTCOMING_MESSAGE_SIZE + 2] = { 0 };
    uint8_t control_data = 0;

    data[0] = 0xFF;

    if (_battle_mode == BattleMode::STATIC && gun_camera_selected) {
      data[1] = _horizontal_position;
      data[2] = _vertical_position;
    } else if (_battle_mode == BattleMode::ARCADE) {
      data[1] = _horizontal_position;
      data[2] = _vertical_position;
    } else {
      data[1] = 0;
      data[2] = 0;
    }

    if (comander_camera_selected) {
      data[3] = _comander_camera_horizontal_position;
      data[4] = _comander_camera_vertical_position;
      control_data |= comander_camera_bind_mode << 0;
    } else {
      data[3] = 0;
      data[4] = 0;
      control_data |= 0;  // Disable bind mode to prevent setting angle as 0. I know... bullshit
    }

    
    control_data |= _laser << 1;

    if (_turret_lights) {
      control_data |= 0xC;   // Left & Right Gun Light
      control_data |= 0x80;  // Commander Light
    }

    if (_warning_led)
      control_data |= 0x40;

    //Fast blinking is not implemented in the turret, so FAST_BLINKING is the same as BLINKING
    if (_turret_lights_mode == LightsMode::BLINKING || _turret_lights_mode == LightsMode::FAST_BLINKING) {
      control_data |= 0x30;  // Set blinking mode for Left & Right Gun lights
    }

    data[5] = control_data;
    data[6] = CEModule.get_command();
    data[7] = get_check_sum(data, OUTCOMING_MESSAGE_SIZE);

    _serial_stream->write(data, OUTCOMING_MESSAGE_SIZE + 2);
  }

  bool _read_serial_message(uint8_t* data) {
    if (_serial_stream->available() != INCOMING_MESSAGE_SIZE + 2)
      return false;

    uint32_t last_update_time = millis();
    uint8_t index = 1;
    bool start = false;

    while (true) {
      if (millis() - last_update_time > 50)
        return false;

      if (_serial_stream->available() == 0)
        continue;

      uint8_t incoming_byte = _serial_stream->read();

      if (start) {

        data[index++] = incoming_byte;
        if (index == INCOMING_MESSAGE_SIZE + 2) {

          // Check the checksum which is the latest item
          if (data[INCOMING_MESSAGE_SIZE + 1] == get_check_sum(data, INCOMING_MESSAGE_SIZE)) {
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
};

#endif