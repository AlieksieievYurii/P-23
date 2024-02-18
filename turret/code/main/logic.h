#ifndef __LOGIC_H__
#define __LOGIC_H__

volatile bool is_calibrated = false;
volatile uint8_t sequence = 0;
volatile uint8_t loader_state = LS_REQUIRES_CALIBRATION;
uint8_t command_execution = 0;
bool command_send = false;
bool shell_connected = false;

void command_execution_tick(void) {
  if (command_send) {
    uint8_t status = Module.get_status();

    if (status == CES_DONE) {
      CommandEngine.finish();
      command_send = false;
      command_execution = 0;
    } else if (status == CES_FAILED) {
      if (command_execution < 0x80)
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
      else
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));

      command_send = false;
      command_execution = 0;
    }
  } else if (command_execution != 0) {
    Module.send_command(command_execution);
    command_send = true;
  }
}

void sm_calibration() {
  uint8_t status = 0;
  switch (sequence) {
    case 0x0:
      shell_connected = false;
      gun_vertical_positioning_servo_control.lock();
      Module.send_command(OPEN_BREECH_GATE_COMMAND);
      sequence = 0x1;
      break;

    case 0x1:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x2;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }

      break;

    case 0x2:
      Module.send_command(LOADING_DRUM_CALIBRATION);
      sequence = 0x3;
      break;

    case 0x3:
      status = Module.get_status();

      if (status == CES_DONE)
        sequence = 0x4;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x4:
      Module.send_command(LOADING_DRUM_CONNECT);
      sequence = 0x5;
      break;

    case 0x5:
      status = Module.get_status();
      if (status == CES_DONE) {
        sequence = 0x6;
      } else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x6:
      Module.send_command(LOADING_DRUM_CALIBRATION);
      sequence = 0x7;
      break;

    case 0x7:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x8;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x8:
      Module.send_command(CLOSE_BREECH_GATE_COMMAND);
      sequence = 0x9;
      break;

    case 0x9:
      status = Module.get_status();
      if (status == CES_DONE) {
        loader_state = LS_UNLOADED;
        is_calibrated = true;
        gun_vertical_positioning_servo_control.unlock();
        CommandEngine.finish();
      } else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;
  }
}

void sm_load() {
  uint8_t status = 0;
  switch (sequence) {

    case 0x0:
      gun_vertical_positioning_servo_control.lock();
      sequence = 0x1;
      loader_state = LS_LOADING;
      break;

    case 0x1:
      Module.send_command(LOADING_DRUM_CONNECT);
      sequence = 0x2;
      break;

    case 0x2:
      status = Module.get_status();

      if (status == CES_DONE)
        sequence = 0x3;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x3:
      Module.send_command(OPEN_BREECH_GATE_COMMAND);
      sequence = 0x4;

    case 0x4:
      status = Module.get_status();

      if (status == CES_DONE)
        sequence = 0x5;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x5:
      Module.send_command(LOADING_DRUM_LOAD_SHELL);
      sequence = 0x6;
      break;

    case 0x6:
      status = Module.get_status();

      if (status == CES_DONE)
        sequence = 0x7;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;
    case 0x7:
      Module.send_command(PRE_CLOSE_BREECH_GATE_COMMAND);
      sequence = 0x8;
      break;

    case 0x8:
      status = Module.get_status();

      if (status == CES_DONE)
        sequence = 0x9;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;
    case 0x9:
      Module.send_command(LOADING_DRUM_FINISH_LOADING);
      sequence = 0xA;
      break;
    case 0xA:
      status = Module.get_status();

      if (status == CES_DONE)
        sequence = 0xB;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;
    case 0xB:
      Module.send_command(CLOSE_BREECH_GATE_COMMAND);
      sequence = 0xC;
      break;
    case 0xC:
      status = Module.get_status();

      if (status == CES_DONE)
        sequence = 0xD;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;
    case 0xD:
      Module.send_command(LOADING_DRUM_DISCONNECT);
      sequence = 0xE;
      break;

    case 0xE:
      status = Module.get_status();

      if (status == CES_DONE) {
        sequence = 0xF;
      } else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0xF:
      Module.send_command(VALIDATE_SHELL);
      sequence = 0x10;
      break;

    case 0x10:
      status = Module.get_status();
      if (status == CES_DONE) {
        sequence = 0x11;
        shell_connected = true;
      } else if (status == CES_FAILED) {
        uint8_t error_code = Module.get_error_code();
        if (error_code == ERROR_NO_SHELL_CONNECTION) {
          sequence = 0x11;
          shell_connected = false;
        } else {
          CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(error_code));
          loader_state = LS_FAILURE;
        }
      }
      break;

    case 0x11:
      loader_state = LS_LOADED;
      CommandEngine.finish();
      gun_vertical_positioning_servo_control.unlock();
      break;
  }
}

void sm_unload() {
  uint8_t status = 0;
  switch (sequence) {
    case 0x0:
      shell_connected = false;
      gun_vertical_positioning_servo_control.lock();
      sequence = 0x1;
      loader_state = LS_UNLOADING;
      break;

    case 0x1:
      Module.send_command(LOADING_DRUM_CONNECT);
      sequence = 0x2;
      break;

    case 0x2:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x3;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x3:
      Module.send_command(OPEN_BREECH_GATE_COMMAND);
      sequence = 0x4;
      break;

    case 0x4:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x5;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x5:
      Module.send_command(LOADING_DRUM_UNLOAD_SHELL);
      sequence = 0x6;
      break;

    case 0x6:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x7;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x7:
      Module.send_command(CLOSE_BREECH_GATE_COMMAND);
      sequence = 0x8;
      break;

    case 0x8:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x9;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x9:
      Module.send_command(LOADING_DRUM_DISCONNECT);
      sequence = 0xA;
      break;

    case 0xA:
      status = Module.get_status();
      if (status == CES_DONE) {
        gun_vertical_positioning_servo_control.unlock();
        CommandEngine.finish();
        loader_state = LS_UNLOADED;
      } else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
  }
}

void sm_reload(void) {
  uint8_t status = 0;
  switch (sequence) {
    case 0x0:
      shell_connected = false;
      gun_vertical_positioning_servo_control.lock();
      sequence = 0x1;
      loader_state = LS_RELOADING;
      break;

    case 0x1:
      Module.send_command(LOADING_DRUM_CONNECT);
      sequence = 0x2;
      break;

    case 0x2:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x3;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x3:
      Module.send_command(OPEN_BREECH_GATE_COMMAND);
      sequence = 0x4;
      break;

    case 0x4:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x5;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x5:
      Module.send_command(LOADING_DRUM_UNLOAD_SHELL);
      sequence = 0x6;
      break;

    case 0x6:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0x7;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x7:
      Module.send_command(LOADING_DRUM_LOAD_SHELL);
      sequence = 0x8;
      break;

    case 0x8:
      status = Module.get_status();
      if (status == CES_DONE) {
        sequence = 0x9;
      } else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x9:
      Module.send_command(PRE_CLOSE_BREECH_GATE_COMMAND);
      sequence = 0xA;
      break;

    case 0xA:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0xB;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0xB:
      Module.send_command(LOADING_DRUM_FINISH_LOADING);
      sequence = 0xC;
      break;

    case 0xC:
      status = Module.get_status();
      if (status == CES_DONE) {
        sequence = 0xD;
      } else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0xD:
      Module.send_command(CLOSE_BREECH_GATE_COMMAND);
      sequence = 0xE;
      break;

    case 0xE:
      status = Module.get_status();
      if (status == CES_DONE)
        sequence = 0xF;
      else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_BREECHBLOCK_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0xF:
      Module.send_command(LOADING_DRUM_DISCONNECT);
      sequence = 0x10;
      break;

    case 0x10:
      status = Module.get_status();
      if (status == CES_DONE) {
        sequence = 0x11;
      } else if (status == CES_FAILED) {
        CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(Module.get_error_code()));
        loader_state = LS_FAILURE;
      }
      break;

    case 0x11:
      Module.send_command(VALIDATE_SHELL);
      sequence = 0x12;
      break;

    case 0x12:
      status = Module.get_status();
      if (status == CES_DONE) {
        sequence = 0x13;
        shell_connected = true;
      } else if (status == CES_FAILED) {
        uint8_t error_code = Module.get_error_code();
        if (error_code == ERROR_NO_SHELL_CONNECTION) {
          sequence = 0x13;
          shell_connected = false;
        } else {
          CommandEngine.fail(WRAP_LOADING_DRUM_ERROR(error_code));
          loader_state = LS_FAILURE;
        }
      }
      break;

    case 0x13:
      loader_state = LS_LOADED;
      CommandEngine.finish();
      gun_vertical_positioning_servo_control.unlock();
      break;
  }
}

#endif