#include <Wire.h>

#include "config.h"
#include "gate.h"

#define DEBUG

Gate gate;

volatile uint8_t command_execution_status = CES_IDLE;
volatile uint8_t error_code = NO_ERROR;
bool shot = false;
uint32_t timestamp = 0;

void setup() {
  Serial.begin(9600);
  init_pins();
  gate.callback = result_callback;
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(on_receive_command);
  Wire.onRequest(on_request_status);
}

void on_request_status() {
  Wire.write((error_code << 3) | command_execution_status);
}

void on_receive_command(uint8_t bytes) {
  if (bytes == 2) {
    lights_state = Wire.read();
    uint8_t command = Wire.read();
    if (command != COMMAND_NOTHING)
      execute_command(command);
  }
}

void execute_command(const uint8_t command) {
  if (command_execution_status != CES_IDLE && command != C_RESET)
    return;

  switch (command) {
    case C_RESET:
      if (command_execution_status == CES_DONE || command_execution_status == CES_FAILED) {
        command_execution_status = CES_IDLE;
        error_code = NO_ERROR;
      }
      break;

    case C_OPEN_GATE:
      command_execution_status = CES_DOING;
      gate.open();
      break;

    case C_PRE_CLOSE_GATE:
      command_execution_status = CES_DOING;
      gate.hold();
      break;

    case C_CLOSE_GATE:
      command_execution_status = CES_DOING;
      gate.close();
      break;

    case C_VALIDATE_SHELL:
      command_execution_status = CES_DOING;
      if (digitalRead(CONTACT_VALIDATOR_PIN))
        result_callback(false, ERROR_NO_SHELL_CONNECTION);
      else
        result_callback(true, NO_ERROR);
      break;

    case C_SHOT:
      command_execution_status = CES_DOING;
      timestamp = millis();
      shot = true;
      break;
  }
}

void loop() {
#ifdef DEBUG
  static int previous_execution_status = -1;
  static int prev_error = -1;

  if (Serial.available()) {
    uint8_t com = Serial.parseInt();
    execute_command(com);
  }

  if (previous_execution_status != command_execution_status) {
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
    previous_execution_status = command_execution_status;
  }

  if (prev_error != error_code) {
    if (error_code != NO_ERROR) {
      Serial.print("Error Occured(Code:");
      Serial.print(error_code);
      Serial.print("):");
    }

    switch (error_code) {
      case ERROR_NOT_ALLOWED_TO_PRE_CLOSE_GATE_FROM_CLOSED_STATE:
        Serial.println("ERROR_NOT_ALLOWED_TO_PRE_CLOSE_GATE_FROM_CLOSED_STATE");
        break;
      case ERROR_GATE_TIMEOUT:
        Serial.println("GATE_TIMEOUT");
        break;
      case ERROR_NO_SHELL_CONNECTION:
        Serial.println("ERROR_NO_SHELL_CONNECTION");
        break;
    }
    prev_error = error_code;
  }
#endif

  gate.tick();
  tick_lights();

  if (shot) {
    digitalWrite(FIRE_RELAY_PIN, HIGH);
    if (millis() - timestamp >= 500) {
      digitalWrite(FIRE_RELAY_PIN, LOW);
      result_callback(true, NO_ERROR);
      shot = false;
    }
  }

  digitalWrite(LED_BUILTIN, command_execution_status == CES_IDLE);
}

void result_callback(const bool successful, const uint8_t err_code) {
  command_execution_status = successful ? CES_DONE : CES_FAILED;
  error_code = err_code;
}
