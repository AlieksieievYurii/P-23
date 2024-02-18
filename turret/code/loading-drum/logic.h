#ifndef __LOGIC_H__
#define __LOGIC_H__

#define COMMAND_DONE \
  command_execution = 0; \
  sm_sequence = 0; \
  command_execution_status = CES_DONE; \
  error_code = 0

#define COMMAND_FAILED \
  command_execution = 0; \
  sm_sequence = 0; \
  command_execution_status = CES_FAILED

volatile uint8_t sm_sequence = 0;

void sm_calibration_command(void) {
  switch (sm_sequence) {
    case 0x0:
      gate.open();
      sm_sequence = 0x1;
      break;

    case 0x1:
      if (gate.get_status() == Status::FINISHED)
        sm_sequence = 0x2;
      else if (gate.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = gate.get_error_code();
      }
      break;

    case 0x2:
      feeder.push();
      sm_sequence = 0x3;
      break;

    case 0x3:
      if (feeder.get_status() == Status::FINISHED)
        sm_sequence = 0x4;
      else if (feeder.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = feeder.get_error_code();
      }
      break;

    case 0x4:
      feeder.pull();
      sm_sequence = 0x5;
      break;

    case 0x5:
      if (feeder.get_status() == Status::FINISHED)
        sm_sequence = 0x6;
      else if (feeder.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = feeder.get_error_code();
      }
      break;

    case 0x6:
      gate.close();
      sm_sequence = 0x7;
      break;

    case 0x7:
      if (gate.get_status() == Status::FINISHED)
        sm_sequence = 0x8;
      else if (gate.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = gate.get_error_code();
      }
      break;

    case 0x8:
      drum_rotation.next();
      sm_sequence = 0x9;
      break;

    case 0x9:
      if (drum_rotation.get_status() == Status::FINISHED)
        sm_sequence = 0xA;
      else if (drum_rotation.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = drum_rotation.get_error_code();
      }
      break;

    case 0xA:
      loading_drum_mover.deactivate();
      sm_sequence = 0xB;
      break;

    case 0xB:
      if (loading_drum_mover.get_status() == Status::FINISHED)
        sm_sequence = 0xC;
      else if (loading_drum_mover.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = loading_drum_mover.get_error_code();
      }
      break;

    case 0xC:
      COMMAND_DONE;
      is_calibrated = true;
      break;
  }
}

void sm_handle_drum_loader_activation(bool activate) {
  if (sm_sequence == 0x0) {
    if (activate)
      loading_drum_mover.activate();
    else
      loading_drum_mover.deactivate();
    sm_sequence = 0x1;
  } else if (sm_sequence == 0x1) {
    if (loading_drum_mover.get_status() == Status::FINISHED) {
      COMMAND_DONE;
    } else if (loading_drum_mover.get_status() == Status::FAILED) {
      COMMAND_FAILED;
      error_code = loading_drum_mover.get_error_code();
    }
  }
}

void sm_load_shell(void) {
  static uint8_t attamp_count = 0;
  switch (sm_sequence) {
    case 0x0:
      if (IS_SHELL) {
        sm_sequence = 0x2;
      } else {
        if (attamp_count < 6) {
          drum_rotation.next();
          sm_sequence = 0x1;
          attamp_count++;
        } else {
          attamp_count = 0;
          COMMAND_FAILED;
          error_code = ERROR_NO_SHELLS_TO_LOAD;
        }
      }
      break;

    case 0x1:
      if (drum_rotation.get_status() == Status::FINISHED)
        sm_sequence = 0x0;
      else if (drum_rotation.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = drum_rotation.get_error_code();
      }
      break;

    case 0x2:
      attamp_count = 0;
      gate.open();
      sm_sequence = 0x3;
      break;

    case 0x3:
      if (gate.get_status() == Status::FINISHED)
        sm_sequence = 0x4;
      else if (gate.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = gate.get_error_code();
      }
      break;

    case 0x4:
      feeder.push();
      sm_sequence = 0x5;
      break;

    case 0x5:
      if (feeder.get_status() == Status::FINISHED) {
        COMMAND_DONE;
      } else if (feeder.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = feeder.get_error_code();
      }
      break;
  }
}

void sm_finish_loading_shell(void) {
  if (sm_sequence == 0x0) {
    feeder.pull();
    sm_sequence = 0x1;
  } else if (sm_sequence == 0x1) {
    if (feeder.get_status() == Status::FINISHED)
      sm_sequence = 0x2;
    else if (feeder.get_status() == Status::FAILED) {
      COMMAND_FAILED;
      error_code = feeder.get_error_code();
    }
  } else if (sm_sequence == 0x2) {
    gate.close();
    sm_sequence = 0x3;
  } else if (sm_sequence == 0x3) {
    if (gate.get_status() == Status::FINISHED) {
      COMMAND_DONE;
    } else if (gate.get_status() == Status::FAILED) {
      COMMAND_FAILED;
      error_code = gate.get_error_code();
    }
  }
}

void sm_unload_shell(void) {
  static uint8_t attamp_count = 0;

  switch (sm_sequence) {
    case 0x0:
      if (IS_SHELL == false) {
        sm_sequence = 0x2;
        attamp_count = 0;
      } else {
        if (attamp_count < 6) {
          drum_rotation.previous();
          sm_sequence = 0x1;
          attamp_count++;
        } else {
          attamp_count = 0;
          COMMAND_FAILED;
          error_code = ERROR_NO_EMPTY_CELL_TO_UNLOAD_BREECHBLOCK;
        }
      }
      break;

    case 0x1:
      if (drum_rotation.get_status() == Status::FINISHED)
        sm_sequence = 0x0;
      else if (drum_rotation.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = drum_rotation.get_error_code();
      }
      break;

    case 0x2:
      gate.open();
      sm_sequence = 0x3;
      break;

    case 0x3:
      if (gate.get_status() == Status::FINISHED)
        sm_sequence = 0x4;
      else if (gate.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = gate.get_error_code();
      }
      break;

    case 0x4:
      feeder.push();
      sm_sequence = 0x5;
      break;

    case 0x5:
      if (feeder.get_status() == Status::FINISHED)
        sm_sequence = 0x6;
      else if (feeder.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = feeder.get_error_code();
      }
      break;

    case 0x6:
      feeder.pull();
      sm_sequence = 0x7;
      break;

    case 0x7:
      if (feeder.get_status() == Status::FINISHED)
        sm_sequence = 0x8;
      else if (feeder.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = feeder.get_error_code();
      }
      break;

    case 0x8:
      if (IS_SHELL) {
        sm_sequence = 0x9;
      } else {
        COMMAND_FAILED;
        error_code = ERROR_COULD_NOT_PULL_SHELL_FROM_BREECHBLOCK;
      }
      break;

    case 0x9:
      gate.close();
      sm_sequence = 0xA;
      break;

    case 0xA:
      if (gate.get_status() == Status::FINISHED) {
        sm_sequence = 0xB;
      } else if (gate.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = gate.get_error_code();
      }
      break;
    
    case 0xB:
      drum_rotation.next();
      sm_sequence = 0xC;
      break;
    
    case 0xC:
      if (drum_rotation.get_status() == Status::FINISHED) {
        COMMAND_DONE;
      } else if (gate.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = gate.get_error_code();
      }
      break;
  }
}

void sm_drum_rotation(bool clockwise) {
  switch (sm_sequence) {
    case 0x0:
      if (clockwise)
        drum_rotation.next();
      else
        drum_rotation.previous();
      sm_sequence = 0x1;
      break;

    case 0x1:
      if (drum_rotation.get_status() == Status::FINISHED) {
        COMMAND_DONE;
      } else if (drum_rotation.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = drum_rotation.get_error_code();
      }
      break;
  }
}

void sm_drum_gate(bool open) {
  switch (sm_sequence) {
    case 0x0:
      if (open)
        gate.open();
      else
        gate.close();
      sm_sequence = 0x1;
      break;

    case 0x1:
      if (gate.get_status() == Status::FINISHED) {
        COMMAND_DONE;
      } else if (gate.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = gate.get_error_code();
      }
      break;
  }
}

void sm_feeder(bool push) {
  switch (sm_sequence) {
    case 0x0:
      if (push)
        feeder.push();
      else
        feeder.pull();
      sm_sequence = 0x1;
      break;

    case 0x1:
      if (feeder.get_status() == Status::FINISHED) {
        COMMAND_DONE;
      } else if (feeder.get_status() == Status::FAILED) {
        COMMAND_FAILED;
        error_code = feeder.get_error_code();
      }
      break;
  }
}

void falsify_calibration(void) {
  is_calibrated = true;
  COMMAND_DONE;
}

#endif