The main module is responsible for the turret control. It communicates with the platform through UART protocol.

|  Address |                     Value                       |                                                           Description                                                                 |
|----------|-------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------|
|   0x00   |               0xFF                                                              | Start-of-Frame delimiter                                  |
|   0x01   | 0x00 - stop; 0x01..7F - left; 0x80..0xFE - right                                  | Turret rotation. Speed grows with larger values.          |
|   0x02   | 0x00 - stop; 0x01..7F - up; 0x80..0xFE - down.                                    | Gun angle. Speed grows with larger values.                |
|   0x03   | 0x00 - stop; 0x01..7F - right; 0x80..0xFE - left. Can be 0x00..0xFF if Bind Mode  | Commander Camera rotation. Speed grows with larger values. If Bind Mode is enabled, then the range(0x00..0xFF) is interpreted as fixed angle (0..180).                                                                          |
|   0x04   | 0x00 - stop; 0x01..7F - up; 0x80..0xFE - down. Can be 0x00..0xFF if Bind Mode      | Commander Camera angle. Speed grows with larger values. If Bind Mode is enabled, then the range(0x00..0xFF) is interpreted as fixed angle (0..180). |
|   0x05   |                                                 | Control data. 1 (first bit) - bind mode
|   0x06   | 0x0..0x5. Positive bit - ON                     | First bit - commander light. Second bit - turret left light. Third bit - turret right light. Fourth bit - laser.Fifth - warning blink |
