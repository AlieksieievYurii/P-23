# Remote Control Panel

This is a remote control panel for the tank. The main communication is done by nRF24 module. The remote panel control sends a packet each 100-200 milliseconds, containing the states(joystick positions, buttons states, config and so on). Refer to Protocol Table.

The module uses two libraries:
* [Radio Head](https://www.airspayce.com/mikem/arduino/RadioHead/index.html) - simple API for nRF24.
* [Arduino Cryptography Library](https://rweather.github.io/arduinolibs/index.html) - the radio connection is encrypted by using this library.

Note: In order to use both library together, you must uncomment the define in the file RadioHead.h of Radio Head library:
![image](https://github.com/AlieksieievYurii/Tank/assets/39415360/ddc2b885-8076-42ce-9455-e1e4e8f61235)

## Protocol table
The data has the size 20 bytes where each byte represents a state.
|  Byte |  Description |
|-------|---------------------------------------|
|  0x0 |  Drive Joystick X. The range is from 0 to 255. |
|  0x1 |  Drive Joystick Y. The range is from 0 to 255. |
|  0x2 |  Drive Joystick R. The range is from 0 to 255. |
|  0x3 |  Arm Turret Joystick X. The range is from 0 to 255. |
|  0x4 |  Arm Turret Joystick Y. The range is from 0 to 255. |
|  0x5 |  Arm Turret Joystick R. The range is from 0 to 255. |
|  0x6 |  Camera Joystick X. The range is from 0 to 255. |
|  0x7 |  Camera Joystick Y. The range is from 0 to 255. |
|  0x8 |  Driving Speed Potentiometer. The range is from 0 to 255. |
|  0x9 |  Arm Turret Speed Potentiometer. The range is from 0 to 255. |
|  0xA |  First 4 bits - command.  |
|  0xB |  Camera Movement Speed Potentiometer. The range is from 0 to 255. |
|  0xC |  Contains light states and camera selection. The first 4 bits represent light states: Each positive bit means switch on. The first bit is _Back Switch_, the second is _Front Switch_, the third is _Long Switch_, and the fourth is _Turret Switch_. The next 3 bits(converting to hex)  represents camera selection, where 0x0 - Front, 0x1 - Back, 0x2 - Turret, 0x3 - Gun. 0x4 - A, 0x5 - R|
|  0xD | Each pair of bits represents light modes(00 - stable light, 01 - blinking, 10 - fast blinking). The first pair - _Back Switch_, the second is _Front Switch_, the third is _Long Switch_, the fourth is _Turret Switch_ |
|  0xE | The first bit is driving mode(0 - B, 1 - A), the second bit represents camera bind(0 - disabled, 1 - enabled), third and fourth bits represent Battle mode (00 - C, 01 - A, 10 - B), fifth bit - reprensents laser, sixth - stabilizer, eighth bit - combat readiness |

# Input

| Byte |                                        Description                                             |
|------|------------------------------------------------------------------------------------------------|
| 0x0  | Battery voltage(integer part)                                                                  |
| 0x1  | Battery voltage(fractional part)                                                               |
| 0x2  | Battery Capacity (the value from 0 to 4)                                                       |
| 0x3  | Current execution command                                                                      |
| 0x4  | Execution status                                                                               |
| 0x5  | Execution error code                                                                           |
| 0x6  | First 3 bits -> Loader state. See [Loader States](#loader-states). Next bit - shell connection |

## Loader States

| State ID |                          Description                       |
|----------|------------------------------------------------------------|
|   0x0    |  Requires calibration                                      |
|   0x1    |  Calibration is being in progress                          |
|   0x2    |  Loading                                                   |
|   0x3    |  Unloading                                                 |
|   0x4    |  Loaded                                                    |
|   0x5    |  Unloaded                                                  |
|   0x6    |  Reloading                                                 |
|   0x7    |  Failure                                                   |

## External Interface

The panel also has an external interface that allows it to connect different modules. The interface uses UART protocol.

Interface pinout:

![image](https://github.com/AlieksieievYurii/Tank/assets/39415360/212ac611-3172-49ab-b9de-fa17ce52816c)


