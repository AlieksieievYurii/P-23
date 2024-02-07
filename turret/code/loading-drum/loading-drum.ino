#include <Wire.h>

#include "config.h"
#include "motor.h"

#define DEBUG

Gate gate;
LoadingDrumMover loading_drum_mover;
DrumRotation drum_rotation;
Feeder feeder;

volatile bool is_calibrated = false;
volatile uint8_t command_execution = 0;
volatile uint8_t command_execution_status = CES_IDLE;
volatile uint8_t error_code = NO_ERROR;

#include "logic.h"

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  init_pins();
  Wire.begin(THIS_DEVICE_I2C_ADDRESS);
  Wire.onReceive(on_receive_command);
  Wire.onRequest(on_request_status);
}

void on_request_status() {
  Wire.write ((error_code << 3) | command_execution_status);
}

void on_receive_command(uint8_t bytes) {
  if (bytes == 1) {
    uint8_t command = Wire.read();
    execute_command(command);
  }
}

void execute_command(uint8_t command) {
  if (command_execution_status == CES_DOING)
    return;

  if (command_execution_status != CES_DOING && command == RESET_STATUS_COMMAND) {
    command_execution = 0;
    command_execution_status = CES_IDLE;
    error_code = NO_ERROR;
    sm_sequence = 0;
  } else {
    if (command != CALIBRATION_COMMAND && command != COMMAND_FALSIFY_CALIBRATION && !is_calibrated) {
      command_execution_status = CES_FAILED;
      error_code = ERROR_CALIBRATION_REQUIRED;
      return;
    }

    if (command_execution_status == CES_IDLE) {
      command_execution = command;
      command_execution_status = CES_DOING;
    }
  }
}

void handle_commands(void) {
  if (command_execution == CALIBRATION_COMMAND)
    sm_calibration_command();
  else if (command_execution == ACTIVATE_LOADER_DRUM)
    sm_handle_drum_loader_activation(true);
  else if (command_execution == DEACTIVATE_LOADER_DRUM)
    sm_handle_drum_loader_activation(false);
  else if (command_execution == LOAD_SHELL)
    sm_load_shell();
  else if (command_execution == COMMAND_FINISH_LOAD_SHELL)
    sm_finish_loading_shell();
  else if (command_execution == COMMAND_UNLOAD_SHELL)
    sm_unload_shell();
  else if (command_execution == COMMAND_CLOCKWISE_DRUM_ROTATION)
    sm_drum_rotation(true);
  else if (command_execution == COMMAND_COUNTER_CLOCKWISE_DRUM_ROTATION)
    sm_drum_rotation(false);
  else if (command_execution == COMMAND_OPEN_DRUM_GATE)
    sm_drum_gate(true);
  else if (command_execution == COMMAND_CLOSE_DRUM_GATE)
    sm_drum_gate(false);
  else if (command_execution == COMMAND_PUSH_FEEDER)
    sm_feeder(true);
  else if (command_execution == COMMAND_PULL_FEEDER)
    sm_feeder(false);
  else if (command_execution == COMMAND_FALSIFY_CALIBRATION)
    falsify_calibration();
}

void loop() {
#ifdef DEBUG
  static int prev_status = -1;
  static int prev_error = -1;
  if (Serial.available() > 0) {
    execute_command(Serial.parseInt());
  }
  if (prev_status != command_execution_status) {
    Serial.print("Execution Status(Code:");
    Serial.print(command_execution_status);
    Serial.print("):");
    switch (command_execution_status) {
      case CES_IDLE:
        Serial.println("IDLE");
        break;
      case CES_DOING:
        Serial.println("DOING");
        break;
      case CES_DONE:
        Serial.println("DONE");
        break;
      case CES_FAILED:
        Serial.println("FAILED");
        break;
    }
    prev_status = command_execution_status;
  }

  if (prev_error != error_code) {
    if (error_code != NO_ERROR) {
      Serial.print("Error Occured(Code:");
      Serial.print(error_code);
      Serial.print("):");
    }

    switch (error_code) {
      case ERROR_CALIBRATION_REQUIRED:
        Serial.println("ERROR_CALIBRATION_REQUIRED");
        break;
      case ERROR_FEEDER_PUSH_TIMEOUT:
        Serial.println("ERROR_FEEDER_PUSH_TIMEOUT");
        break;
      case ERROR_FEEDER_PULL_TIMEOUT:
        Serial.println("ERROR_FEEDER_PULL_TIMEOUT");
        break;
      case ERROR_LOADING_DRUM_ACTIVATION_TIMEOUT:
        Serial.println("ERROR_LOADING_DRUM_ACTIVATION_TIMEOUT");
        break;
      case ERROR_LOADING_DRUM_DEACTIVATION_TIMEOUT:
        Serial.println("ERROR_LOADING_DRUM_DEACTIVATION_TIMEOUT");
        break;
      case ERROR_GATE_OPENING_TIMEOUT:
        Serial.println("ERROR_GATE_OPENING_TIMEOUT");
        break;
      case ERROR_GATE_CLOSING_TIMEOUT:
        Serial.println("ERROR_GATE_CLOSING_TIMEOUT");
        break;
      case ERROR_NO_SHELLS_TO_LOAD:
        Serial.println("ERROR_NO_SHELLS_TO_LOAD");
        break;
      case ERROR_NO_EMPTY_CELL_TO_UNLOAD_BREECHBLOCK:
        Serial.println("ERROR_NO_EMPTY_CELL_TO_UNLOAD_BREECHBLOCK");
        break;
      case ERROR_COULD_NOT_PULL_SHELL_FROM_BREECHBLOCK:
        Serial.println("ERROR_COULD_NOT_PULL_SHELL_FROM_BREECHBLOCK");
        break;
      case ERROR_DRUM_CLOCKWISE_ROTATION_TIMEOUT:
        Serial.println("ERROR_DRUM_CLOCKWISE_ROTATION_TIMEOUT");
        break;
      case ERROR_DRUM_COUNTER_CLOCKWISE_ROTATION_TIMEOUT:
        Serial.println("ERROR_DRUM_COUNTER_CLOCKWISE_ROTATION_TIMEOUT");
    }
    prev_error = error_code;
  }
#endif

  loading_drum_mover.tick();
  gate.tick();
  drum_rotation.tick();
  feeder.tick();
  handle_commands();

  digitalWrite(LED_BUILTIN, command_execution_status == CES_IDLE);
}
