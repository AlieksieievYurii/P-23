#ifndef __MODULE_H__
#define __MODULE_H__

#include <Wire.h>

#define LOADER_DRUM_MODULE_ADDRESS 0x01
#define GUN_MODULE_ADDRESS 0x02

// Loading Drum Module Commands
#define LOADING_DRUM_CALIBRATION 0x01
#define LOADING_DRUM_CONNECT 0x02
#define LOADING_DRUM_DISCONNECT 0x03
#define LOADING_DRUM_LOAD_SHELL 0x04
#define LOADING_DRUM_FINISH_LOADING 0x05
#define LOADING_DRUM_UNLOAD_SHELL 0x06
#define LOADING_DRUM_CLOCKWISE_DRUM_ROTATION 0x07
#define LOADING_DRUM_COUNTER_CLOCKWISE_DRUM_ROTATION 0x08
#define LOADING_DRUM_OPEN_GATE 0x09
#define LOADING_DRUM_CLOSE_GATE 0x0A
#define LOADING_DRUM_PUSH_FEEDER 0x0B
#define LOADING_DRUM_PULL_FEEDER 0x0C
#define LOADING_DRUM_FALSIFY_CALIBRATION 0x0D
// TODO
// ------------------------------------------

// Gun Module
#define OPEN_BREECH_GATE_COMMAND 0x81
#define PRE_CLOSE_BREECH_GATE_COMMAND 0x82
#define CLOSE_BREECH_GATE_COMMAND 0x83
#define VALIDATE_SHELL 0x84
#define GUN_FIRE_COMMAND 0x85
//---------------------------------------

// Command Execution Status
#define CES_NONE 0
#define CES_DOING 1
#define CES_DONE 2
#define CES_FAILED 3
// -------------------------

#define NO_ERROR 0

class _Module {
public:
  _Module() {
    Wire.begin();
  }

  /*
  * Sends command to some of two modules. 
  * Returns True if there is already some command being executed.
  */
  bool send_command(uint8_t command) {
    if (_command_execution_status != CES_NONE)
      return true;

    if (command < 0x80)
      _send_command_to_loading_drum(command);
    else
      _send_command_to_gun_module(command);
    _command_execution = command;
    _command_execution_status = CES_DOING;
    _timestamp = millis();
    return false;
  }

  void set_lights_state(const uint8_t value) {
    static int prev_value = -1;
    if (prev_value != value) {
      uint8_t result = 0;
      result |= value >> 1;
      result = (result | 96) ^ 96;  // Sets two last bits as zero
      _lights_state = result;

      Wire.beginTransmission(GUN_MODULE_ADDRESS);
      Wire.write(_lights_state);
      Wire.write(0xFF);
      Wire.endTransmission();
    }
  }

  const uint8_t get_status(void) {
    uint8_t buf = _command_execution_status;
    if (buf == CES_DONE || buf == CES_FAILED)
      _command_execution_status = CES_NONE;
    return buf;
  }

  const uint8_t see_current_status(void) {
    return _command_execution_status;
  }

  const uint8_t get_error_code(void) {
    uint8_t buf = _error_code;
    _error_code = 0;
    return buf;
  }

  void tick() {
    if (_command_execution != 0 && millis() - _timestamp > 500) {
      Wire.requestFrom(_command_execution < 0x80 ? LOADER_DRUM_MODULE_ADDRESS : GUN_MODULE_ADDRESS, 1);

      if (Wire.available()) {
        uint8_t status_and_error_code = (uint8_t)Wire.read();
        uint8_t status = status_and_error_code & 0x7;  // Read first 3 bits
        if (_is_reset_send && status == CES_NONE) {
          _is_reset_send = false;
          _command_execution_status = _command_execution_status_buf;
          _command_execution = 0;
        } else if (status == CES_DONE || status == CES_FAILED) {
          _command_execution_status_buf = status;
          if (status == CES_FAILED)
            _error_code = status_and_error_code >> 3;
          _send_reset(_command_execution < 0x80 ? LOADER_DRUM_MODULE_ADDRESS : GUN_MODULE_ADDRESS);
          _is_reset_send = true;
        }
      }
    }
  }

private:

  void _send_command_to_gun_module(const uint8_t command) {
    Wire.beginTransmission(GUN_MODULE_ADDRESS);
    Wire.write(_lights_state);
    Wire.write(command - 0x80);
    Wire.endTransmission();
  }

  void _send_command_to_loading_drum(const uint8_t command) {
    Wire.beginTransmission(LOADER_DRUM_MODULE_ADDRESS);
    Wire.write(command);
    Wire.endTransmission();
  }

  void _send_reset(const uint8_t address) {
    Wire.beginTransmission(address);
    if (address == GUN_MODULE_ADDRESS)
      Wire.write(_lights_state);
    Wire.write(0x0);
    Wire.endTransmission();
  }

  volatile uint8_t _prev_lights_state = 0;
  volatile uint8_t _lights_state = 0;
  volatile uint32_t _timestamp = 0;
  uint8_t _error_code = NO_ERROR;
  uint8_t _command_execution = 0;
  bool _is_reset_send = false;
  volatile uint8_t _command_execution_status_buf = CES_NONE;
  volatile uint8_t _command_execution_status = CES_NONE;
};

_Module Module;

#endif