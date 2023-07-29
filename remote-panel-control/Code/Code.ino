#include "config.h"
#include "display.h"

#include <SPI.h>
#include <RH_NRF24.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

Display display;

RH_NRF24 nrf24(CHIP_ENABLE_IN, CHIP_SELECT_IN);
Speck cipher;
RHEncryptedDriver driver(nrf24, cipher);

#define READ_ANALOG_VALUE(pin) map(analogRead(pin), 0, 1023, 0, 255)

uint8_t get_lights_data() {
  uint8_t value = 0;
  value |= IS_SWITCH_ON(LIGHT_SWITCH_BACK);
  value |= IS_SWITCH_ON(LIGHT_SWITCH_FRONT) << 1;
  value |= IS_SWITCH_ON(LIGHT_SWITCH_LONG) << 2;
  value |= IS_SWITCH_ON(LIGHT_SWITCH_TURRET) << 3;

  return value;
}

uint8_t* get_data() {
  static uint8_t data[13] = { 0 };
  data[0] = READ_ANALOG_VALUE(DRIVE_JOY_X);
  data[1] = READ_ANALOG_VALUE(DRIVE_JOY_Y);
  data[2] = READ_ANALOG_VALUE(DRIVE_JOY_R);

  data[3] = READ_ANALOG_VALUE(ARM_TURRET_JOY_X);
  data[4] = READ_ANALOG_VALUE(ARM_TURRET_JOY_Y);
  data[5] = READ_ANALOG_VALUE(ARM_TURRET_JOY_R);

  data[6] = READ_ANALOG_VALUE(CAMERA_JOY_X);
  data[7] = READ_ANALOG_VALUE(CAMERA_JOY_Y);

  data[8] = READ_ANALOG_VALUE(DRIVING_SPEED_POTENTIOMETER);
  data[9] = READ_ANALOG_VALUE(ARM_TURRET_SPEED_POTENTIOMETER);
  data[10] = READ_ANALOG_VALUE(PROG_POTENTIOMETER);
  data[11] = READ_ANALOG_VALUE(CAMERA_MOVEMENT_SPEED_POTENTIOMETER);
  data[12] = get_lights_data();

  return data;
}


void setup() {
  init_pins();
  Serial.begin(9600);

  if (!nrf24.init())
    Serial.println("init failed");

  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");

  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");

  cipher.setKey(ENCRYPTION_KEY, sizeof(ENCRYPTION_KEY));

  display.init();
  display.showSplash();
}

void loop() {
  display.test();
  delay(5000);
  //Serial.println("Sending to nrf24_encrypted_server");
  // Send a message to nrf24_server
  uint8_t* data = get_data();
  // for (int i = 0; i < 13; i++) {
  //   Serial.print(data[i]);
  // }
  // Serial.println();
  // delay(2000);

  driver.send(data, 13);

  driver.waitPacketSent();
  // // Now wait for a reply
  // uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  // uint8_t len = sizeof(buf);

  // if (driver.waitAvailableTimeout(500)) {
  //   // Should be a reply message for us now
  //   if (driver.recv(buf, &len)) {
  //     Serial.print("got reply: ");
  //     Serial.println((char*)buf);
  //   } else {
  //     Serial.println("recv failed");
  //   }
  // } else {
  //   Serial.println("No reply, is nrf24_encrypted_server running?");
  // }
}
