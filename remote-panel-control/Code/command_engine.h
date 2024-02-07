#ifndef __COMMAND_ENGINE_H__
#define __COMMAND_ENGINE_H__

#define CE_NONE 0
#define CE_ACKNOWLEDGED 1
#define CE_IN_PROGRESS 2
#define CE_FINISHED 3
#define CE_FAILED 4

enum class CEStatus {
  IDLE,
  DOING,
  DONE,
  FAILED
};

#define START_EXECUTING_COMMAND 0x1

// Commands
#define COMMAND_LOAD 0x2
#define COMMAND_UNLOAD 0x3
#define COMMAND_RELOAD 0x10
#define COMMAND_MAKE_SHOT 0x12
//---------------------------------

#define NO_COMMAND 0x0
#define NO_ERROR 0x0

// -------------Loading Drum Errors--------------
#define ERROR_CALIBRATION_REQUIRED 0x1

#define ERROR_GATE_OPENING_TIMEOUT 0x2
#define ERROR_GATE_CLOSING_TIMEOUT 0x3

#define ERROR_LOADING_DRUM_ACTIVATION_TIMEOUT 0x4
#define ERROR_LOADING_DRUM_DEACTIVATION_TIMEOUT 0x5

#define ERROR_FEEDER_PUSH_TIMEOUT 0x6
#define ERROR_FEEDER_PULL_TIMEOUT 0x7

#define ERROR_DRUM_CLOCKWISE_ROTATION_TIMEOUT 0x8
#define ERROR_DRUM_COUNTER_CLOCKWISE_ROTATION_TIMEOUT 0x9

#define ERROR_NO_SHELLS_TO_LOAD 0xA
#define ERROR_NO_EMPTY_CELL_TO_UNLOAD_BREECHBLOCK 0xB
#define ERROR_COULD_NOT_PULL_SHELL_FROM_BREECHBLOCK 0xC

//---------Breechblock Errors------------
#define ERROR_NOT_ALLOWED_TO_PRE_CLOSE_GATE_FROM_CLOSED_STATE 0x1
#define ERROR_GATE_TIMEOUT 0x2


// Loader State
#define LS_REQUIRES_CALIBRATION 0
#define LS_CALIBRATION 1
#define LS_LOADING 2
#define LS_UNLOADING 3
#define LS_LOADED 4
#define LS_UNLOADED 5
#define LS_RELOADING 6
#define LS_FAILURE 7


#define ERROR_FAILED_TO_LOAD_FROM_WRONG_STATE 0x1
#define ERROR_FAILED_TO_UNLOAD_FROM_WRONG_STATE 0x2
#define ERROR_FAILED_RELOAD_WHILE_NOT_LOADED 0x3

char* repr_CE_status(CEStatus status) {
  static char* idle = "IDLE";
  static char* doing = "DOING";
  static char* done = "DONE";
  static char* failed = "FAILED";

  switch (status) {
    case CEStatus::IDLE:
      return idle;
    case CEStatus::DOING:
      return doing;
    case CEStatus::DONE:
      return done;
    case CEStatus::FAILED:
      return failed;
  }
}

char* repr_CE_error(const uint8_t error_code) {
  const uint8_t error = error_code >> 2;

  static char* calibration_required = "Calibration required";
  static char* loading_drum_gate_opening_timeout = "LDG Opening Timeout";
  static char* loading_drum_gate_closing_timeout = "LDG Closing Timeout";
  static char* activate_loading_drum_timeout = "LD Act Timeout";
  static char* deactivate_loading_drum_timeout = "LD Deact Timeout";
  static char* feeder_push_timeout = "Feeder Push Timeout";
  static char* feeder_pull_timeout = "Feeder Pull Timeout";
  static char* drum_clockwise_rotation_timeout = "Drum CR Timeout";
  static char* drum_counter_clockwise_rotation_timeout = "Drum CCR Timeout";
  static char* no_shell_to_load = "No Shell To Load";
  static char* no_empty_cell_to_unload_breechblock = "No empty cell UNL";
  static char* could_not_pull_shell_from_breechblock = "Can not pull shell";
  static char* not_allowed_to_pre_close_gate_from_closed_state = "Can not pre-close";
  static char* breechgate_gate_timeout = "Breech Gate Timeout";
  static char* failed_to_load_from_wrong_state = "Failed Load BLS";
  static char* failed_to_unload_from_wrong_state = "Failed Unload BLS";
  static char* failed_reload_while_not_loaded = "Failed Reload BLS";
  static char* no_message = "No message";

  if ((error_code & 0x3) == 0x1) {
    switch (error) {
      case ERROR_CALIBRATION_REQUIRED:
        return calibration_required;
      case ERROR_GATE_OPENING_TIMEOUT:
        return loading_drum_gate_opening_timeout;
      case ERROR_GATE_CLOSING_TIMEOUT:
        return loading_drum_gate_closing_timeout;
      case ERROR_LOADING_DRUM_ACTIVATION_TIMEOUT:
        return activate_loading_drum_timeout;
      case ERROR_LOADING_DRUM_DEACTIVATION_TIMEOUT:
        return deactivate_loading_drum_timeout;
      case ERROR_FEEDER_PUSH_TIMEOUT:
        return feeder_push_timeout;
      case ERROR_FEEDER_PULL_TIMEOUT:
        return feeder_pull_timeout;
      case ERROR_DRUM_CLOCKWISE_ROTATION_TIMEOUT:
        return drum_clockwise_rotation_timeout;
      case ERROR_DRUM_COUNTER_CLOCKWISE_ROTATION_TIMEOUT:
        return drum_counter_clockwise_rotation_timeout;
      case ERROR_NO_SHELLS_TO_LOAD:
        return no_shell_to_load;
      case ERROR_NO_EMPTY_CELL_TO_UNLOAD_BREECHBLOCK:
        return no_empty_cell_to_unload_breechblock;
      case ERROR_COULD_NOT_PULL_SHELL_FROM_BREECHBLOCK:
        return could_not_pull_shell_from_breechblock;
      default:
        return no_message;
    }
  } else if ((error_code & 0x3) == 0x2) {
    switch (error) {
      case ERROR_NOT_ALLOWED_TO_PRE_CLOSE_GATE_FROM_CLOSED_STATE:
        return not_allowed_to_pre_close_gate_from_closed_state;
      case ERROR_GATE_TIMEOUT:
        return breechgate_gate_timeout;

      default:
        return no_message;
    }
  } else if ((error_code & 0x3) == 0x0 && error != 0) {
    switch (error) {
      case ERROR_FAILED_TO_LOAD_FROM_WRONG_STATE:
        return failed_to_load_from_wrong_state;
      case ERROR_FAILED_TO_UNLOAD_FROM_WRONG_STATE:
        return failed_to_unload_from_wrong_state;
      case ERROR_FAILED_RELOAD_WHILE_NOT_LOADED:
        return failed_reload_while_not_loaded;
    }
  } else
    return no_message;
}

char* repr_loader_state(const uint8_t loader_state) {
  static char* requires_calibration = "Requires CAL";
  static char* calibration = "Calibration";
  static char* loading = "Loading";
  static char* unloading = "Unloading";
  static char* loaded = "Loaded";
  static char* unloaded = "Unloaded";
  static char* reloading = "Reloading";
  static char* failure = "Failure";

  switch (loader_state) {
    case LS_REQUIRES_CALIBRATION:
      return requires_calibration;
    case LS_CALIBRATION:
      return calibration;
    case LS_LOADING:
      return loading;
    case LS_UNLOADING:
      return unloading;
    case LS_LOADED:
      return loaded;
    case LS_UNLOADED:
      return unloaded;
    case LS_RELOADING:
      return reloading;
    case LS_FAILURE:
      return failure;
  }
}
/*
* This chain of code is responsible for stateful communication with the platform.
* In order to send some command - call send()
* Command ID must be greater than 1 - because 1 is flag command, meaning that 
* the engine is awaiting for the response.
* Don't forget to call get_status() because this function is also responsible
* for reseting the engine once status is fetched.
* You can not send another command is the previous one is not finished!
*/
class _CommandEngine {
public:
  void send(const uint8_t command) {
    if (command > 1 && _status == CE_NONE && _require_ack == false) {
      _command = command;
      _executing_task = command;
      _require_ack = true;
      _error_code = NO_ERROR;
    }
  }

  const uint8_t get_command(void) {
    return _command;
  }

  const uint8_t get_current_task(void) {
    return _executing_task;
  }

  /*
    * You must always call get_status in order 
    * to reset the engine so that it can accept another command
    */
  CEStatus get_status() {
    if ((_status == CE_NONE && _require_ack == true) || _status == CE_IN_PROGRESS)
      return CEStatus::DOING;
    else if (_status == CE_NONE)
      return CEStatus::IDLE;
    else if (_status == CE_FINISHED) {
      _command = NO_COMMAND;  // Also means to reset the state on the target
      return CEStatus::DONE;
    } else if (_status == CE_FAILED) {
      _command = NO_COMMAND;  // Also means to reset the state on the target
      return CEStatus::FAILED;
    }
  }

  void set_incoming_data(const uint8_t command, const uint8_t status, const uint8_t error_code) {
    if (_is_initialized == false) {
      if (status == CE_ACKNOWLEDGED) {
        _require_ack = true;
      }
      _executing_task = command;
      _is_initialized = true;
    }

    _status = status;

    if (_require_ack && status == CE_ACKNOWLEDGED) {
      _command = START_EXECUTING_COMMAND;
      _require_ack = false;
    } else if (status == CE_FAILED) {
      _error_code = error_code;
    }
  }

  const uint8_t get_last_error_code() {
    return _error_code;
  }

private:
  uint8_t _status = CE_NONE;
  bool _require_ack = false;
  uint8_t _command = NO_COMMAND;
  uint8_t _error_code = NO_ERROR;
  uint8_t _executing_task = 0;
  bool _is_initialized = false;
};

_CommandEngine CommandEngine;

#endif