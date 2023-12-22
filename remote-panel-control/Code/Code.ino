#include <SPI.h>
#include <RH_NRF24.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

#include "buzzer.h"
#include "config.h"
#include "display.h"

RH_NRF24 nrf24(CHIP_ENABLE_IN, CHIP_SELECT_IN);
Speck cipher;
RHEncryptedDriver driver(nrf24, cipher);

#define READ_ANALOG_VALUE(pin) map(analogRead(pin), 0, 1023, 0, 255)

#define IN_RANGE(x, A, B) A <= x&& x <= B

#define SEND_PACKAGE_SIZE 15

bool camera_bind_mode = true;
bool camera_joy_button_flag = false;

uint8_t get_control_data() {
  uint8_t value = 0;
  value |= IS_SWITCH_ON(DRIVING_MODE_SWITCH);
  value |= camera_bind_mode << 1;

  uint8_t battle_mode = 0;
  uint16_t battle_mode_analog_value = analogRead(BATTLE_MODE_DOUBLE_SWITCH);
  if (battle_mode_analog_value <= 341)
    battle_mode = 0; // Battle mode C
  else if (battle_mode_analog_value > 342 && battle_mode_analog_value <= 684)
    battle_mode = 1; // Battle mode A
  else if (battle_mode_analog_value >= 685)
    battle_mode = 2; // Battle mode B
  
  value |= battle_mode << 2;

  return value;
}

uint32_t ping = 0;

uint8_t get_lights_data_and_camera_selection() {
  uint8_t value = 0;
  value |= IS_SWITCH_ON(LIGHT_SWITCH_BACK);
  value |= IS_SWITCH_ON(LIGHT_SWITCH_FRONT) << 1;
  value |= IS_SWITCH_ON(LIGHT_SWITCH_LONG) << 2;
  value |= IS_SWITCH_ON(LIGHT_SWITCH_TURRET) << 3;

  uint16_t camera_select_value = analogRead(CAMERA_SWITCH);
  if (IN_RANGE(camera_select_value, 916, 976))  // Camera Front
    value |= 0x0 << 4;
  else if (IN_RANGE(camera_select_value, 789, 849))  // Camera Back
    value |= 0x1 << 4;
  else if (IN_RANGE(camera_select_value, 645, 705))  // Camera Turret
    value |= 0x2 << 4;
  else if (IN_RANGE(camera_select_value, 551, 611))  // Camera Gun
    value |= 0x3 << 4;
  else if (IN_RANGE(camera_select_value, 481, 541))  // Camera A
    value |= 0x4 << 4;
  else if (IN_RANGE(camera_select_value, 424, 484))  // Camera R
    value |= 0x5 << 4;

  return value;
}

uint8_t get_light_modes() {
  uint8_t value = 0;
  value |= ConfigHolderInstance.back_light_mode;
  value |= ConfigHolderInstance.front_light_mode << 2;
  value |= ConfigHolderInstance.long_light_mode << 4;
  value |= ConfigHolderInstance.turret_light_mode << 6;

  return value;
}

uint8_t* get_data() {
  static uint8_t data[SEND_PACKAGE_SIZE] = { 0 };
  data[0x0] = READ_ANALOG_VALUE(DRIVE_JOY_X);
  data[0x1] = READ_ANALOG_VALUE(DRIVE_JOY_Y);
  data[0x2] = READ_ANALOG_VALUE(DRIVE_JOY_R);

  data[0x3] = READ_ANALOG_VALUE(ARM_TURRET_JOY_X);
  data[0x4] = READ_ANALOG_VALUE(ARM_TURRET_JOY_Y);
  data[0x5] = READ_ANALOG_VALUE(ARM_TURRET_JOY_R);

  data[0x6] = READ_ANALOG_VALUE(CAMERA_JOY_X);
  data[0x7] = READ_ANALOG_VALUE(CAMERA_JOY_Y);

  data[0x8] = READ_ANALOG_VALUE(DRIVING_SPEED_POTENTIOMETER);
  data[0x9] = READ_ANALOG_VALUE(ARM_TURRET_SPEED_POTENTIOMETER);
  data[0xA] = READ_ANALOG_VALUE(PROG_POTENTIOMETER);
  data[0xB] = READ_ANALOG_VALUE(CAMERA_MOVEMENT_SPEED_POTENTIOMETER);

  data[0xC] = get_lights_data_and_camera_selection();
  data[0xD] = get_light_modes();
  data[0xE] = get_control_data();

  return data;
}


void setup() {
  init_pins();
  Serial.begin(9600);

  if (!nrf24.init()) {
    BuzzerInstance.error();
    Serial.println("init failed");
  }


  if (!nrf24.setChannel(1)) {
    BuzzerInstance.error();
    Serial.println("setChannel failed");
  }


  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) {
    BuzzerInstance.error();
    Serial.println("setRF failed");
  }

  cipher.setKey(ENCRYPTION_KEY, sizeof(ENCRYPTION_KEY));

  DisplayInstance.init(ConfigHolderInstance);
  DisplayInstance.showSplash();
}

void handle_display_buttons() {
  static uint32_t _display_ok_timestamp = 0;
  static bool display_left_but_flag = false;
  static bool display_right_but_flag = false;
  static bool display_ok_but_flag = false;


  if (!display_right_but_flag && IS_SWITCH_ON(DISPLAY_RIGHT_BUTTON))
    display_right_but_flag = true;
  else if (display_right_but_flag && !IS_SWITCH_ON(DISPLAY_RIGHT_BUTTON)) {
    DisplayInstance.next_menu_item();
    BuzzerInstance.beep();
    display_right_but_flag = false;
  }

  if (!display_left_but_flag && IS_SWITCH_ON(DISPLAY_LEFT_BUTTON))
    display_left_but_flag = true;
  else if (display_left_but_flag && !IS_SWITCH_ON(DISPLAY_LEFT_BUTTON)) {
    DisplayInstance.previous_menu_item();
    BuzzerInstance.beep();
    display_left_but_flag = false;
  }

  if (!display_ok_but_flag && _display_ok_timestamp == 0 && IS_SWITCH_ON(DISPLAY_OK_CANCEL_BUTTON)) {
    _display_ok_timestamp = millis();
    display_ok_but_flag = true;
  } else if (display_ok_but_flag && IS_SWITCH_ON(DISPLAY_OK_CANCEL_BUTTON) && millis() - _display_ok_timestamp > 1000) {
    display_ok_but_flag = false;
    DisplayInstance.back();
    BuzzerInstance.beep();
  } else if (display_ok_but_flag && !IS_SWITCH_ON(DISPLAY_OK_CANCEL_BUTTON)) {
    DisplayInstance.select();
    BuzzerInstance.beep();
    display_ok_but_flag = false;
  } else if (!IS_SWITCH_ON(DISPLAY_OK_CANCEL_BUTTON)) {
    _display_ok_timestamp = 0;
  }
}

void handle_response(byte* response) {
  DisplayInstance.battery_voltage = (float)response[0] + (float)response[1] / 100.0;
  DisplayInstance.battery_indicator = response[2];
}


void loop() {
  static uint32_t last_received_packed_time = millis();
  static uint32_t disconnection_time = 0;

  if (IS_SWITCH_ON(CAMERA_JOY_BUTTON) && !camera_joy_button_flag) {
    camera_joy_button_flag = true;
  } else if (!IS_SWITCH_ON(CAMERA_JOY_BUTTON) && camera_joy_button_flag) {
    camera_bind_mode = !camera_bind_mode;
    camera_joy_button_flag = false;
  }

  uint8_t* data = get_data();
  // for (int i = 0; i < SEND_PACKAGE_SIZE; i++) {
  //   Serial.print(data[i]);
  // }
  // Serial.println();
  // delay(2000);

  driver.send(data, SEND_PACKAGE_SIZE);
  driver.waitPacketSent();


  // // Now wait for a reply
  byte buf[RH_NRF24_MAX_MESSAGE_LEN] = { 0 };
  uint8_t len = sizeof(buf);

  if (driver.available()) {
    disconnection_time = 0;
    DisplayInstance.connected = true;
    if (driver.recv(buf, &len)) {
      handle_response(buf);
      ping = millis() - last_received_packed_time;
      last_received_packed_time = millis();
    } else {
      Serial.println("recv failed");
    }
  } else {
    if (disconnection_time == 0)
      disconnection_time = millis();

    if (millis() - disconnection_time > DISCONNECTION_TIMEOUT)
      DisplayInstance.connected = false;
  }

  handle_display_buttons();

  if (ConfigHolderInstance.is_communication_channel_changed()) {
    Serial.println(ConfigHolderInstance.communication_channel);
    if (!nrf24.setChannel(ConfigHolderInstance.communication_channel))
      Serial.println("setChannel failed");
  }

  BuzzerInstance.tick();
  DisplayInstance.tick();
  DisplayInstance.ping = ping;

  if (ping > ORANGE_PING_DELAY) {
    digitalWrite(CONNECTION_STATIUS_LED_RED_PIN, HIGH);
    digitalWrite(CONNECTION_STATIUS_LED_GREEN_PIN, HIGH);
  } else if (ping > RED_PING_DELAY) {
    digitalWrite(CONNECTION_STATIUS_LED_RED_PIN, HIGH);
    digitalWrite(CONNECTION_STATIUS_LED_GREEN_PIN, LOW);
  } else {
    digitalWrite(CONNECTION_STATIUS_LED_RED_PIN, LOW);
    digitalWrite(CONNECTION_STATIUS_LED_GREEN_PIN, LOW);
  }
}
