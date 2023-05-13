
#include "config.h"

#include <SPI.h>
#include <RH_NRF24.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>

RH_NRF24 nrf24;
Speck cipher;
RHEncryptedDriver driver(nrf24, cipher);
unsigned char encryptkey[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};


void setup() {
  Serial.begin(9600);

  if (!nrf24.init())
    Serial.println("init failed");

  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");

  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed"); 
    
  cipher.setKey(encryptkey, sizeof(encryptkey));   
}

void loop() {
  if (driver.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (driver.recv(buf, &len))
    {
//      RH_NRF24::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
      
      // Send a reply
      uint8_t data[] = "And hello back"; // Dont make this too long
      driver.send(data, sizeof(data));
      driver.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }

}
