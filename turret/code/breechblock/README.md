# Gun

This is a separated module based on Arduino Pro Mini which is responsible for opening/closing the breechblock gate,
the gun lights, laser and fire process.

Communication with the module is done by I2C protocol. The module is considered as a slave.
The API is very simple - only one byte containing the command to perform and the lights state.
Please refer to [Commands Table](#command-table) and [Lights State](#lights-state).
Please note, after each send command, RESET command must be send.

## Electronic Circuit

![Electronic Circuit](turret-gun-module-circuit.svg)

## Commands Table
As API, it expects one byte where 3 first bits represent the command to perform and next 5 bits
represent the lights states. 
Moreover, the modules constantly sends an [execution status]() of the given command. You can not
send another command if the previuos one is not finished, however you can send a cancel command.
The following table describes the commands:

| Command |                                                  Description                                                        |
|---------|---------------------------------------------------------------------------------------------------------------------|
|  0x00   | Reset status. Must be send once another status is fetched. Status will be reseted only if it is finished or failed  |
|  0x01   | Opens the breechblock gate completely                                                                               |
|  0x02   | Pre closes the breechblock gate so that the shell is fixed                                                          |
|  0x03   | Closes  the breechblock gate completely                                                                             |
|  0x04   | Makes a shot (200ms duration)      TODO                                                                             |
|  0xFF   | Does nothing. This is used to set lights state.                                                                     |

## Lights State

TODO
The one byte, which is supposed to be send, has to contain the lights state,
starting from the third bit and to the last.
The following table describes the point of each bit:

| Bit |                      Description                           |
|-----|------------------------------------------------------------|
|  1  |  _Relates to the command_                                  |               
|  2  |  _Relates to the command_                                  |               
|  3  |  _Relates to the command_                                  |               
|  4  |  Gun Laser (positive bit enables that)                     |               
|  5  |  Left Gun Light (positive bit enables that)                |               
|  6  |  Right Gun Light (positive bit enables that)               |               
|  7  |  Left Gun Light Mode. 1 - blinking mode. 0 - constant mode |
|  8  |  Right Gun Light Mode. 1 - blinking mode. 0 - constant mode|

## Output

The response from the module is one byte. The first 3 bits represents the execution status.
The following table describes the statuses:

| Status |                 Description               |
|--------|-------------------------------------------|
|  0x0   | Idle. Means you can send commands         |
|  0x1   | Command execution is in progress          |
|  0x2   | Command execution is finished successfuly |
|  0x3   | Command execution is failed               |

The next 4 bits represents code errors. The code error appears only if the execution status is failed.
The following table describes the code errors:

|  Code  |                         Description                         |
|--------|-------------------------------------------------------------|
|  0x0   | No code error                                               |
|  0x1   | Not allowed to pre-close the breechblock from closed state  |
|  0x2   | Gate timeout                                                |
