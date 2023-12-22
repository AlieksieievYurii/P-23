
Packet size: ?

|  Address |                     Value                       |                                                           Description                                                                 |
|----------|-------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------|
|   0x00   |               0xFF                              | Start-of-Frame delimiter                                                                                                              |
|   0x01   | 0x00 - stop; 0x01..7F - left; 80..0xFE - right  | Turret rotation. Speed grows with larger values.                                                                                      |
|   0x02   | 0x00 - stop; 0x01..7F - up; 80..0xFE - down     | Gun angle. Speed grows with larger values.                                                                                            |
|   0x03   | 0x00 - stop; 0x01..7F - right; 80..0xFE - left  | Commander Cammera rotation. Speed grows with larger values                                                                            |
|   0x04   | 0x00 - stop; 0x01..7F - up; 80..0xFE - down     | Commander Cammera angle. Speed grows with larger values                                                                               |
|   0x05   | 0x0..0x5. Positive bit - ON                     | First bit - commander light. Second bit - turret left light. Third bit - turret right light. Fourth bit - laser.Fifth - warning blink |