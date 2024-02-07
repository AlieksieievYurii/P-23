
#include "config.h"

#include <SPI.h>
#include <RH_NRF24.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

#include "driving.h"
#include "lights.h"
#include "camera.h"
#include "logger.h"
#include "battery.h"
#include "turret.h"
#include "command_engine.h"


RH_NRF24 nrf24(CHIP_ENABLE_IN, CHIP_SELECT_IN);
Speck cipher;
RHEncryptedDriver driver(nrf24, cipher);
Logger logger(STATUS_LED_PIN, BUZZER_PIN, Serial);
Driving driving;
Battery battery(BATERY_VOLTAGE_PIN);

Turret turret(Serial2);


volatile float buf[10] = { 0 };

void handle_p_23_turret(const uint8_t* const data) {
  const uint8_t turret_joy_x = data[0x3];
  const uint8_t turret_joy_y = data[0x4];
  const uint8_t control_data = data[0xE];
  const uint8_t camera_joy_x = data[0x6];
  const uint8_t camera_joy_y = data[0x7];
  bool camera_mode_bind = data[0xE] & 0x2;

  uint8_t camera_movement_speed = data[0xB];
  uint8_t turret_movement_speed = data[0x9];

  turret.gun_camera_selected = current_selected_camera == GUN_CAMERA_ID;
  turret.comander_camera_selected = current_selected_camera == TURRET_CAMERA_ID;
  turret.comander_camera_bind_mode = camera_mode_bind;

  turret.set_horizontal_position(turret_joy_x, turret_movement_speed);
  turret.set_vertical_position(turret_joy_y, turret_movement_speed);
  turret.set_battle_mode(control_data);
  turret.set_comander_camera_vertical_position(camera_joy_y, camera_movement_speed);
  turret.set_comander_camera_horizontal_position(camera_joy_x, camera_movement_speed);
  turret.set_laser(data[0xE]);
  turret.set_warning_led(data[0xE]);
  turret.set_lights(data[0xC], data[0xD]);
}

void handle_package(uint8_t* data) {
#ifdef DEBUG
  uint8_t len = 14;
  Serial.print("[DEBUG] Got data: ");
  for (int i = 0; i < len; i++)
    Serial.print(data[i]);
  Serial.println();
#endif

  uint8_t drive_joy_x = data[0x0];
  uint8_t drive_joy_y = data[0x1];
  uint8_t drive_joy_r = data[0x2];
  uint8_t drive_speed = data[0x8];

  uint8_t camera = data[0x6];
  uint8_t camera_selection = data[0xC] >> 4;
  bool inertial_driving = data[0xE] & 0x1;
  bool camera_mode_bind = data[0xE] & 0x2;

  uint8_t gun_ready_mode = (data[0xE] >> 6) & 0x1;

  uint8_t gun_state = 0;
  if (gun_ready_mode && turret.shell_connected)
    gun_state = 2;
  else if (gun_ready_mode)
    gun_state = 1;

  CommandEngine.set_incoming_data(data[0xA]);

  handle_front_camera_control(camera, camera_mode_bind, data[0xB]);
  handle_lights(data);
  driving.handle_driving(drive_joy_x, drive_joy_y, drive_joy_r, drive_speed, inertial_driving);


  switch (handle_camera_selection(camera_selection)) {
    case FRONT_CAMERA_ID:
      osd.set_front_view(driving.left_track_power, driving.right_track_power, driving.left_track_forward_direction, driving.right_track_forward_direction, (uint8_t)map(camera, 0, 255, 0, 10));
      break;
    case BACK_CAMERA_ID:
      osd.set_back_view(driving.left_track_power, driving.right_track_power, driving.left_track_forward_direction, driving.right_track_forward_direction);
      break;
    case TURRET_CAMERA_ID:
      osd.set_commander_view(driving.left_track_power, driving.right_track_power, driving.left_track_forward_direction, driving.right_track_forward_direction, (uint8_t)map(camera, 0, 255, 0, 10));
      break;
    case GUN_CAMERA_ID:
      osd.set_gunner_view(driving.left_track_power, driving.right_track_power, driving.left_track_forward_direction, driving.right_track_forward_direction, turret.get_loader_state(), gun_state);
      break;
  }
}


void send_package() {
  static byte data[7] = { 0 };
  data[0x0] = battery.voltage_intpart;
  data[0x1] = battery.voltage_decpart;
  data[0x2] = battery.capacity;
  data[0x3] = CommandEngine.get_command();
  data[0x4] = CommandEngine.get_status();
  data[0x5] = CommandEngine.get_error_code();
  data[0x6] = (turret.shell_connected << 3) | turret.get_loader_state();

  driver.send(data, sizeof(data));
  driver.waitPacketSent();
}

void setup() {
  init_pins();
  Serial.begin(9600);
  Serial3.begin(9600);
  Serial2.begin(9600);

  front_camera_servo.attach(FRONT_CAMERA_SERVO_PIN);

  if (!nrf24.init())
    logger.set_state(TRANSCEIVER_INIT_ERROR);

  if (!nrf24.setChannel(1))
    logger.set_state(TRANSCEIVER_SET_CHANEL_ERROR);

  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    logger.set_state(TRANSCEIVER_SET_CHANEL_ERROR);

  cipher.setKey(ENCRYPTION_KEY, sizeof(ENCRYPTION_KEY));

  delay(5000);
  battery.init();
}

void handle_commutication() {
  if (driver.available()) {
    uint8_t buffer[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buffer);

    if (driver.recv(buffer, &len)) {
      handle_package(buffer);
#ifdef TURRET_P_23_ENABLE
      handle_p_23_turret(buffer);
#endif
      send_package();
    } else {
      Serial.println("recv failed");
    }
  } else {
    //Serial.println("Timeout");
    //driving.enable_motors(false);
  }
}

void loop() {
  driving.enable_motors(true);
  handle_commutication();

  lights.tick();
  logger.tick();
  battery.tick();
  turret.tick();

  if (CommandEngine.is_task()) {
    CEModule.send(CommandEngine.get_task());
  }

  CEStatus s = CEModule.get_status();
  if (s == CEStatus::DONE) {
    CommandEngine.finish();
  } else if (s == CEStatus::FAILED) {
    CommandEngine.fail(CEModule.get_error_code());
  }
}
