#ifndef __TURRET_H__
#define __TURRET_H__

#include <Arduino.h>

#define MESSAGE_SIZE 4

enum class BattleMode {
  SAFE,
  ARCADE,
  STATIC
};

const uint8_t get_check_sum(const uint8_t* data) {
  uint16_t sum = 0;
  for (uint8_t i = 1; i <= MESSAGE_SIZE; i++)
    sum += data[i];
  return sum / MESSAGE_SIZE;
}

class Turret {
public:
  bool gun_camera_selected = false;
  bool comander_camera_selected = false;

  Turret(HardwareSerial& serial_stream) {
    _serial_stream = &serial_stream;
  }

  /*
  * Turret rotation. Value must be in the range 0..255.
  * If the value is from 1..127 -> the turret turns left
  * If the value is from 128..255 -> the turret turns right
  */
  void set_horizontal_position(uint8_t value) {
    if (value >= 0 && value <= 120) {
      _horizontal_position = map(value, 120, 0, 0x0, 0x7F);
    } else if (value >= 135 && value <= 255) {
      _horizontal_position = map(value, 135, 255, 0x80, 0xFE);
    } else {
      _horizontal_position = 0;
    }
  }

  /* Gun angle. Value must be in the range 0..255.
  * If the value is from 1..127 -> the gun goes down.
  * If the value is from 128..255 -> the gun goes up
  */
  void set_vertical_position(uint8_t value) {
    if (value >= 0 && value <= 120) {
      _vertical_position = map(value, 120, 0, 0x0, 0x7F);
    } else if (value >= 135 && value <= 255) {
      _vertical_position = map(value, 135, 255, 0x80, 0xFE);
    } else {
      _vertical_position = 0;
    }
  }

  void set_comander_camera_vertical_position(uint8_t value) {
    if (value >= 0 && value <= 120) {
      _comander_camera_vertical_position = map(value, 120, 0, 0x0, 0x7F);
    } else if (value >= 130 && value <= 255) {
      _comander_camera_vertical_position = map(value, 130, 255, 0x80, 0xFE);
    } else {
      _comander_camera_vertical_position = 0;
    }
  }

  void set_comander_camera_horizontal_position(uint8_t value) {
    if (value >= 0 && value <= 120) {
      _horizontal_position = map(value, 120, 0, 0x0, 0x7F);
    } else if (value >= 130 && value <= 255) {
      _horizontal_position = map(value, 130, 255, 0x80, 0xFE);
    } else {
      _horizontal_position = 0;
    }
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

  void tick() {
    uint8_t data[MESSAGE_SIZE + 2] = { 0 };
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
    } else {
      data[3] = 0;
      data[4] = 0;
    }

    data[5] = get_check_sum(data);
    _serial_stream->write(data, MESSAGE_SIZE + 2);
  }

private:
  BattleMode _battle_mode = BattleMode::SAFE;
  HardwareSerial* _serial_stream;
  volatile uint8_t _horizontal_position = 0;
  volatile uint8_t _vertical_position = 0;
  volatile uint8_t _comander_camera_vertical_position = 0;
  volatile uint8_t _comander_camera_horizontal_position = 0;
};

#endif