#include "config.h"

#include <SPI.h>
#include <RH_NRF24.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

RH_NRF24 nrf24(CHIP_ENABLE_IN, CHIP_SELECT_IN);
Speck cipher;
RHEncryptedDriver driver(nrf24, cipher);

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
}

void loop() {
  Serial.println("Sending to nrf24_encrypted_server");
  // Send a message to nrf24_server
  uint8_t data[] = { 0x2, 0x1, 0xff, 0x0f, 0x04 };
  driver.send(data, sizeof(data));

  driver.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (driver.waitAvailableTimeout(500)) {
    // Should be a reply message for us now
    if (driver.recv(buf, &len)) {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
    } else {
      Serial.println("recv failed");
    }
  } else {
    Serial.println("No reply, is nrf24_encrypted_server running?");
  }
  delay(500);
}
