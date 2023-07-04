# Radio Control Receiver

The module uses two libraries:
* [Radio Head](https://www.airspayce.com/mikem/arduino/RadioHead/index.html) - simple API for nRF24.
* [Arduino Cryptography Library](https://rweather.github.io/arduinolibs/index.html) - the radio connection is encrypted by using this library.

Note: In order to use both library together, you must uncomment the define in the file RadioHead.h of Radio Head library:
![image](https://github.com/AlieksieievYurii/Tank/assets/39415360/ddc2b885-8076-42ce-9455-e1e4e8f61235)
