//#define DEBUG

#include "config.h"
#include "servo-control.h"
#include <GyverTimers.h>
#include "rotation-control.h"
#include "api.h"
#include "module.h"
#include "command_engine.h"

Servo gun_vertical_positioning_servo;
Servo commander_camera_horizontal_servo;
Servo commander_camera_vertical_servo;
GunAngle gun_vertical_positioning_servo_control(gun_vertical_positioning_servo);
TurretRotation turret_horizontal_rotation(TURRET_ROTATION_DIR_PIN, TURRET_ROTATION_STEP_PIN);
CommanderServo commander_camera_horizontal_control(commander_camera_horizontal_servo, 20, 160);
CommanderServo commander_camera_vertical_control(commander_camera_vertical_servo);

#include "logic.h"

bool warning_blink = false;

ISR(TIMER2_A) {
  turret_horizontal_rotation.interupt_call();
}

void setup() {
  Serial.begin(9600);
  init_pins();
  gun_vertical_positioning_servo.attach(GUN_VERTICAL_SERVO_PIN);
  commander_camera_horizontal_servo.attach(COMMANDER_CAMERA_HORIZONTAL_SERVO_PIN);
  commander_camera_vertical_servo.attach(COMMANDER_CAMERA_VERTICAL_SERVO_PIN);
  delay(2000);
}

void handle_camera(const uint8_t* const incoming_data) {
  bool comander_camera_bind_mode = incoming_data[5] & 0x1;

  if (comander_camera_bind_mode)
    commander_camera_horizontal_control.set(incoming_data[3]);
  else
    commander_camera_horizontal_control.write(incoming_data[3]);

  if (comander_camera_bind_mode)
    commander_camera_vertical_control.set(incoming_data[4]);
  else
    commander_camera_vertical_control.write(incoming_data[4]);
}

void handle_input(void) {
  uint8_t incoming_data[INCOMING_MESSAGE_SIZE + 2] = { 0 };
  if (read_serial_message(incoming_data)) {
    turret_horizontal_rotation.write(incoming_data[1]);
    gun_vertical_positioning_servo_control.write(incoming_data[2]);
    handle_camera(incoming_data);
    digitalWrite(COMMANDER_CAMERA_LIGHTS_PIN, incoming_data[5] >> 7 & 0x1);
    Module.set_lights_state(incoming_data[5]);
    CommandEngine.set_incoming_data(incoming_data[6]);
    warning_blink = (incoming_data[5] >> 6) & 0x1;
  }
}

void handle_warnign_blink(void) {
  static uint32_t timestamp = 0;
  static bool flag = false;
  if (warning_blink) {
    if (millis() - timestamp >= 500) {
      digitalWrite(WARNING_LED_PIN, flag);
      flag = !flag;
      timestamp = millis();
    }
  } else {
    digitalWrite(WARNING_LED_PIN, LOW);
  }
}

#ifdef DEBUG
void run_debug(const uint8_t command) {
  if (command == 0) {
    Module.get_status();
  } else if (command == 1) {
    Module.send_command(OPEN_BREECH_GATE_COMMAND);
  } else if (command == 2) {
    Module.send_command(CLOSE_BREECH_GATE_COMMAND);
  } else if (command == 3) {
    Module.send_command(PRE_CLOSE_BREECH_GATE_COMMAND);
  } else if (command == 4) {
    Module.send_command(LOADING_DRUM_OPEN_GATE);
  } else if (command == 5) {
    Module.send_command(LOADING_DRUM_CLOSE_GATE);
  } else if (command == 6) {
    Module.send_command(LOADING_DRUM_CALIBRATION);
  } else if (command == 7) {
    set_state(LS_LOADING);
  } else if (command == 8) {
    set_state(LS_UNLOADING);
  }
}

void print_debug(void) {
  static int prev_ce_status = -1;
  static int prev_ce_error_code = -1;
  static int prev_loader_state = -1;
  static int prev_shell_connected = -1;
  if (CommandEngine.get_status() != prev_ce_status) {
    prev_ce_status = CommandEngine.get_status();
    Serial.print("CE Status:");
    switch (prev_ce_status) {
      case CE_NONE:
        Serial.println("NONE");
        break;
      case CE_ACKNOWLEDGED:
        Serial.println("ACKNOWLEDGED");
        break;
      case CE_IN_PROGRESS:
        Serial.println("IN_PROGRESS");
        break;
      case CE_FINISHED:
        Serial.println("FINISHED");
        break;
      case CE_FAILED:
        Serial.println("FAILED");
        break;
    }
  }

  if (shell_connected != prev_shell_connected) {
    Serial.print("Shell Connection: ");
    Serial.println(shell_connected);
    prev_shell_connected = shell_connected;
  }

  if (CommandEngine.get_error_code() != prev_ce_error_code) {
    prev_ce_error_code = CommandEngine.get_error_code();
    Serial.print("CE Error:");
    Serial.println(prev_ce_error_code);
  }

  if (prev_loader_state != loader_state) {
    Serial.print("Loader State: ");
    switch (loader_state) {
      case LS_CALIBRATION:
        Serial.println("CALIBRATION");
        break;
      case LS_LOADING:
        Serial.println("LOADING");
        break;
      case LS_UNLOADING:
        Serial.println("UNLOADING");
        break;
      case LS_LOADED:
        Serial.println("LOADED");
        break;
      case LS_UNLOADED:
        Serial.println("UNLOADED");
        break;
      case LS_RELOADING:
        Serial.println("RELOADING");
        break;
    }
    prev_loader_state = loader_state;
  }
}
#endif

void loop() {
#ifndef DEBUG
  handle_input();
#else
  if (Serial.available() > 0) {
    int command = Serial.parseInt();
    CommandEngine.set_command(command);
    //run_debug(command);
  }
  print_debug();
#endif

  gun_vertical_positioning_servo_control.tick();
  turret_horizontal_rotation.tick();
  commander_camera_horizontal_control.tick();
  commander_camera_vertical_control.tick();
  Module.tick();
  command_execution_tick();
  handle_warnign_blink();

  if (CommandEngine.is_task()) {
    switch (CommandEngine.get_task()) {
      case TASK_LOAD:
        if (loader_state == LS_UNLOADED)
          set_state(LS_LOADING);
        else
          CommandEngine.fail(ERROR_FAILED_TO_LOAD_FROM_WRONG_STATE);
        break;
      case TASK_UNLOAD:
        if (loader_state == LS_LOADED)
          set_state(LS_UNLOADING);
        else
          CommandEngine.fail(ERROR_FAILED_TO_UNLOAD_FROM_WRONG_STATE);
        break;

      case TASK_CALIBRATE:
        set_state(LS_CALIBRATION);
        break;

      case TASK_OPEN_LOADING_DRUM_GATE:
        set_command_execution(LOADING_DRUM_OPEN_GATE);
        break;

      case TASK_CLOSE_LOADING_DRUM_GATE:
        set_command_execution(LOADING_DRUM_CLOSE_GATE);
        break;

      case TASK_CONNECT_LOADING_DRUM:
        set_command_execution(LOADING_DRUM_CONNECT);
        break;

      case TASK_DISCCONNECT_LOADING_DRUM:
        set_command_execution(LOADING_DRUM_DISCONNECT);
        break;

      case TASK_ROTATE_LOADING_DRUM_CLOCKWISE:
        set_command_execution(LOADING_DRUM_CLOCKWISE_DRUM_ROTATION);
        break;

      case TASK_ROTATE_LOADING_DRUM_COUNTER_CLOCKWISE:
        set_command_execution(LOADING_DRUM_COUNTER_CLOCKWISE_DRUM_ROTATION);
        break;

      case TASK_PUSH_FEEDER_LOADING_DRUM:
        set_command_execution(LOADING_DRUM_PUSH_FEEDER);
        break;

      case TASK_PULL_FEEDER_LOADING_DRUM:
        set_command_execution(LOADING_DRUM_PULL_FEEDER);
        break;

      case TASK_OPEN_BREECHBLOCK_GATE:
        set_command_execution(OPEN_BREECH_GATE_COMMAND);
        break;

      case TASK_CLOSE_BREECHBLOCK_GATE:
        set_command_execution(CLOSE_BREECH_GATE_COMMAND);
        break;

      case TASK_PRE_CLOSE_BREECHBLOCK_GATE:
        set_command_execution(PRE_CLOSE_BREECH_GATE_COMMAND);
        break;

      case TASK_RELOAD:
        if (loader_state == LS_LOADED)
          set_state(LS_RELOADING);
        else
          CommandEngine.fail(ERROR_FAILED_RELOAD_WHILE_NOT_LOADED);
        break;

      case TASK_FALCIFY_CALIBRATION:
        loader_state = LS_UNLOADED;
        set_command_execution(LOADING_DRUM_FALSIFY_CALIBRATION);
        break;

      case TASK_MAKE_SHOT:
        set_command_execution(GUN_FIRE_COMMAND);
        shell_connected = false;
        break;
    }
  }

  if (loader_state == LS_CALIBRATION) {
    sm_calibration();
  } else if (loader_state == LS_LOADING) {
    sm_load();
  } else if (loader_state == LS_UNLOADING) {
    sm_unload();
  } else if (loader_state == LS_RELOADING) {
    sm_reload();
  }

  send_outcoming_data(CommandEngine.get_command(), CommandEngine.get_status(), CommandEngine.get_error_code(), loader_state, shell_connected);
}

void set_state(const uint8_t state) {
  sequence = 0;
  loader_state = state;
}

void set_command_execution(const uint8_t command) {
  sequence = 0;
  command_execution = command;
}
