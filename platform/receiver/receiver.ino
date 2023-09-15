
#include "config.h"

#include <SPI.h>
#include <RH_NRF24.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

#include "driving.h"
#include "lights.h"
#include "camera.h"

RH_NRF24 nrf24(CHIP_ENABLE_IN, CHIP_SELECT_IN);
Speck cipher;
RHEncryptedDriver driver(nrf24, cipher);


void handle_package(uint8_t* data) {
  //uint8_t len = 13;
  // Serial.print("got request: ");
  // for (int i = 0; i < len; i++)
  //   Serial.print(data[i]);

  // Serial.println();

  uint8_t drive_joy_x = data[0];
  uint8_t drive_joy_y = data[1];
  uint8_t drive_joy_r = data[2];

  uint8_t drive_speed = data[8];

  uint8_t camera = data[6];

  uint8_t lights_states = data[12];
  
  handle_front_camera_control(camera);
  handle_lights(lights_states);
  handle_driving(drive_joy_x, drive_joy_y, drive_joy_r, drive_speed, true);
}

void send_package() {
      uint8_t data[] = "And hello back";  // Dont make this too long
      driver.send(data, sizeof(data));
      driver.waitPacketSent();
}

void setup() {
  init_pins();
  Serial.begin(9600);

  front_camera_servo.attach(FRONT_CAMERA_SERVO_PIN);
  
  if (!nrf24.init())
    Serial.println("init failed");

  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");

  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  cipher.setKey(ENCRYPTION_KEY, sizeof(ENCRYPTION_KEY));
}

void handle_commutication() {
   if (driver.waitAvailableTimeout(TIMEOUT)) {
    uint8_t buffer[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buffer);
    if (driver.recv(buffer, &len)) {
      Serial.println("Res");
      handle_package(buffer);
      send_package();
    } else {
      Serial.println("recv failed");
    }
  } else { 
    Serial.println("Timeout");
    DISABLE_TRACKS();
  }
}

void loop() {
  ENABLE_TRACKS();
  handle_commutication();
}
