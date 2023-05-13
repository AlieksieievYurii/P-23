# Tank
This is my project where I've decided to create DIY tank platform.

# Project Structure
* `platform` - the folder contains sources and docs regarding the platform itself.
* `rc` - the folder contains sources and docs regarding radio controll panel

# IMPORTANT
In this project, the connection between the tank and radio control panel is done by nRF24 module.
The following libraries are used:
* [Radio Head](https://www.airspayce.com/mikem/arduino/RadioHead/index.html) - simple API for nRF24.
* [Arduino Cryptography Library](https://rweather.github.io/arduinolibs/index.html) - the radio connection is encrypted by using this library.

Note: In order to use both library together, you must uncomment the define in the file RadioHead.h of Radio Head library:
![image](https://github.com/AlieksieievYurii/Tank/assets/39415360/ddc2b885-8076-42ce-9455-e1e4e8f61235)
