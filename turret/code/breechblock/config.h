#ifndef __CONFIG_H__
#define __CONFIG_H__

#define FIRE_RELAY_PIN 7
#define CONTACT_VALIDATOR_PIN 4
#define BREECH_BLOCK_GATE_MOTOR_CLOSING_PIN 6
#define BREECH_BLOCK_GATE_MOTOR_OPENING_PIN 9
#define BREECH_BLOCK_GATE_IR_PIN 3
#define BREECH_BLOCK_GATE_CONSTRAIN_END_PIN 2
#define LASER_PIN 12
#define BAREL_LEFT_LIGHTS_PIN 11
#define BAREL_RIGHT_LIGHTS_PIN 10
#define BREECH_BLOCK_AND_LOADING_DRUM_CONNECTION_CHECKER_PIN 5

#define BLINKING_INTERVAL_MS 50

#define I2C_ADDRESS 0x2

// Commands
#define C_RESET 0x0
#define C_OPEN_GATE 0x1
#define C_PRE_CLOSE_GATE 0x2
#define C_CLOSE_GATE 0x3
#define C_VALIDATE_SHELL 0x4
#define C_SHOT 0x5
#define COMMAND_NOTHING 0xFF

// Command Execution Status
#define CES_IDLE 0
#define CES_DOING 1
#define CES_DONE 2
#define CES_FAILED 3
// -------------------------

// Error codes
#define NO_ERROR 0x0
#define ERROR_NOT_ALLOWED_TO_PRE_CLOSE_GATE_FROM_CLOSED_STATE 0x1
#define ERROR_GATE_TIMEOUT 0x2
#define ERROR_NO_SHELL_CONNECTION 0x3

void init_pins() {
  pinMode(FIRE_RELAY_PIN, OUTPUT);
  pinMode(CONTACT_VALIDATOR_PIN, INPUT_PULLUP);
  pinMode(BREECH_BLOCK_GATE_MOTOR_CLOSING_PIN, OUTPUT);
  pinMode(BREECH_BLOCK_GATE_MOTOR_OPENING_PIN, OUTPUT);
  pinMode(BREECH_BLOCK_GATE_IR_PIN, INPUT);
  pinMode(BREECH_BLOCK_GATE_CONSTRAIN_END_PIN, INPUT_PULLUP);
  pinMode(LASER_PIN, OUTPUT);
  pinMode(BAREL_LEFT_LIGHTS_PIN, OUTPUT);
  pinMode(BAREL_RIGHT_LIGHTS_PIN, OUTPUT);
}

uint8_t lights_state = 0;

void tick_lights() {
  static uint32_t timestamp = 0;
  static bool enable_timer = false;

  digitalWrite(LASER_PIN, lights_state & 1);

  if (!(lights_state >> 3 & 1))
    digitalWrite(BAREL_LEFT_LIGHTS_PIN, lights_state >> 1 & 1);
  
  if (!(lights_state >> 4 & 1))
    digitalWrite(BAREL_RIGHT_LIGHTS_PIN, lights_state >> 2 & 1);

  if (millis() - timestamp > BLINKING_INTERVAL_MS) {
    if (lights_state >> 3 & 1) { // If Blink mode enabled
      if (lights_state >> 1 & 1) // Left Light is turned on
        digitalWrite(BAREL_LEFT_LIGHTS_PIN, !digitalRead(BAREL_LEFT_LIGHTS_PIN));
      else
        digitalWrite(BAREL_LEFT_LIGHTS_PIN, LOW);
    }
    
    if (lights_state >> 4 & 1) {
      if (lights_state >> 2 & 1)
        digitalWrite(BAREL_RIGHT_LIGHTS_PIN, !digitalRead(BAREL_RIGHT_LIGHTS_PIN));
      else
      digitalWrite(BAREL_RIGHT_LIGHTS_PIN, LOW);
    }
      
    timestamp = millis();
  }
}

#endif