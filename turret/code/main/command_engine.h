#ifndef __COMMAND_ENGINE_H__
#define __COMMAND_ENGINE_H__

#define CE_NONE 0
#define CE_ACKNOWLEDGED 1
#define CE_IN_PROGRESS 2
#define CE_FINISHED 3
#define CE_FAILED 4

// Define wrappers that add bit describing to which module the error is related to
#define WRAP_LOADING_DRUM_ERROR(ERROR_CODE) (ERROR_CODE << 2) | 1
#define WRAP_BREECHBLOCK_ERROR(ERROR_CODE) (ERROR_CODE << 2) | 2
// --------------------

#define ERROR_FAILED_TO_LOAD_FROM_WRONG_STATE (0x1 << 2) | 0
#define ERROR_FAILED_TO_UNLOAD_FROM_WRONG_STATE (0x2 << 2) | 0
#define ERROR_FAILED_RELOAD_WHILE_NOT_LOADED (0x3 << 2) | 0

class _CommandEngine {
public:
  void set_incoming_data(const uint8_t command) {
    if (command > 1 && _status == CE_NONE) {
      _command = command;
      _status = CE_ACKNOWLEDGED;
    } else if (command == 1 && _status == CE_ACKNOWLEDGED) {
      _allow = true;
    } else if (command == 0 && (_status == CE_FINISHED || _status == CE_FAILED)) {
      _status = CE_NONE;
      _command = 0;
      _error_code = 0;
    }
  }

#ifdef DEBUG
  void set_command(const uint8_t command) {
    if (_status == CE_NONE || _status == CE_FINISHED || _status == CE_FAILED) {
      _allow = true;
      _command = command;
      _error_code = 0;
    }
  }
#endif

  bool is_task() {
    return _command != 0 && _allow;
  }

  uint8_t get_task() {
    _status = CE_IN_PROGRESS;
    _allow = false;
    return _command;
  }

  void finish() {
    if (_status == CE_IN_PROGRESS)
      _status = CE_FINISHED;
  }

  void fail(const uint8_t error_code) {
    if (_status == CE_IN_PROGRESS) {
      _status = CE_FAILED;
      _error_code = error_code;
    }
  }

  const uint8_t get_command() {
    return _command;
  }

  const uint8_t get_status() {
    return _status;
  }

  const uint8_t get_error_code(void) {
    return _error_code;
  }

private:
  uint8_t _status = CE_NONE;
  uint8_t _command = 0;
  bool _allow = false;
  uint8_t _error_code = 0;
};

_CommandEngine CommandEngine;

#endif