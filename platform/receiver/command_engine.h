#ifndef __COMMAND_ENGINE_H__
#define __COMMAND_ENGINE_H__

#define CE_NONE 0
#define CE_ACKNOWLEDGED 1
#define CE_IN_PROGRESS 2
#define CE_FINISHED 3
#define CE_FAILED 4

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

  bool is_task() {
    return _command != 0 && _allow;
  }

  const uint8_t get_command() {
    return _command;
  }

  const uint8_t get_error_code() {
    return _error_code;
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

  const uint8_t get_status() {
    return _status;
  }

private:
  uint8_t _status = CE_NONE;
  uint8_t _command = 0;
  uint8_t _error_code = 0;
  bool _allow = false;
};

_CommandEngine CommandEngine;

enum class CEStatus {
  IDLE,
  DOING,
  DONE,
  FAILED
};

#define COMMAND_LOAD 0x2
#define COMMAND_UNLOAD 0x3

/*
* This chain of code is responsible for stateful communication with the platform.
* In order to send some command - call send()
* Command ID must be greater than 1 - because 1 is flag command, meaning that 
* the engine is awaiting for the response.
* Don't forget to call get_status() because this function is also responsible
* for reseting the engine once status is fetched.
* You can not send another command is the previous one is not finished!
*/
class _CommandEngineClient {
  public:
    void send(const uint8_t command) {
      Serial.print("Send Command: ");
      Serial.println(command);
        if (command > 1 && _status == CE_NONE && _require_ack == false) {
          _command = command;
          _require_ack = true;
        }
    }
  
    const uint8_t get_command() {
      return _command;
    }

    /*
    * You must always call get_status in order 
    * to reset the engine so that it can accept another command
    */
    CEStatus get_status() {
      if (_status == CE_NONE)
        return CEStatus::IDLE;
      else if ((_status == CE_NONE && _require_ack == true) || _status == CE_IN_PROGRESS)
        return CEStatus::DOING;
      else if (_status == CE_FINISHED) {
        _command = 0;
        return CEStatus::DONE;
      }
      else if (_status == CE_FAILED) {
         _command = 0;
         return CEStatus::FAILED;
      }
        
    }

    void set_incoming_data(const uint8_t command, const uint8_t status, const uint8_t error_code) {
      _status = status;
      _last_error_code = error_code;

      static int prev_s = -1;
      if (prev_s != status) {
        Serial.print("Received status: ");
        Serial.println(status);
        Serial.println(error_code);
        prev_s = status;
      }

      if (_require_ack && status == CE_ACKNOWLEDGED) {
          _command = 1;
          _require_ack = false;
      }
    }

  const uint8_t get_error_code() {
    return _last_error_code;
  }
  
  private:
    uint8_t _status = CE_NONE;
    bool _require_ack = false;
    uint8_t _command = 0;
    uint8_t _last_error_code = 0;
};

#endif