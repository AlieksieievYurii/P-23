#ifndef __CONFIG_H__
#define __CONFIG_H__

//#define DEBUG

//------- Transiver Config ---------
#define CHIP_ENABLE_IN 49
#define  CHIP_SELECT_IN 48

#define TIMEOUT 500

//----------------------------------

//------- Right Motor Driver -------
#define RIGHT_MOTOR_DRIVE_R_IS 24
#define RIGHT_MOTOR_DRIVE_L_IS 25
#define RIGHT_MOTOR_DRIVE_R_EN 22
#define RIGHT_MOTOR_DRIVE_L_EN 23
#define RIGHT_MOTOR_DRIVE_RPWM 9
#define RIGHT_MOTOR_DRIVE_LPWM 10
//-----------------------------------

//-------- Left Motor Driver --------
#define LEFT_MOTOR_DRIVE_R_IS 31
#define LEFT_MOTOR_DRIVE_L_IS 32
#define LEFT_MOTOR_DRIVE_R_EN 29
#define LEFT_MOTOR_DRIVE_L_EN 30
#define LEFT_MOTOR_DRIVE_RPWM 7
#define LEFT_MOTOR_DRIVE_LPWM 6
//----------------------------------

//--------- Lights Config ----------
#define LEFT_BACK_LIGHT_PIN 33
#define LEFT_FRONT_LIGHT_PIN 34

#define RIGHT_BACK_LIGHT_PIN 37
#define RIGHT_FRONT_LIGHT_PIN 38

#define LEFT_LONG_LIGHT_PIN 35
#define RIGHT_LONG_LIGHT_PIN 36
//---------------------------------

#define FRONT_CAMERA_SERVO_PIN 11

#define BUZZER_PIN 3

//----- Camera output switches ----------
#define FRONT_CAMERA_SWITCH_PIN 28
#define BACK_CAMERA_SWITCH_PIN 27
#define TURRET_CAMERA_SWITCH_PIN 26
#define GUN_CAMERA_SWITCH_PIN 41
#define A_CAMERA_SWITCH_PIN 40
#define R_CAMERA_SWITCH_PIN 39
//---------------------------------------

#define BATERY_VOLTAGE_PIN A0

#define STATUS_LED_PIN 13

#define TURRET_P_23_ENABLE

unsigned char ENCRYPTION_KEY[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

void init_pins() {
  pinMode(RIGHT_MOTOR_DRIVE_R_IS, INPUT);
  pinMode(RIGHT_MOTOR_DRIVE_L_IS, INPUT);
  pinMode(LEFT_MOTOR_DRIVE_R_IS, INPUT);
  pinMode(LEFT_MOTOR_DRIVE_L_IS, INPUT);

  pinMode(RIGHT_MOTOR_DRIVE_R_EN, OUTPUT);
  pinMode(RIGHT_MOTOR_DRIVE_L_EN, OUTPUT);
  pinMode(RIGHT_MOTOR_DRIVE_RPWM, OUTPUT);
  pinMode(RIGHT_MOTOR_DRIVE_LPWM, OUTPUT);

  pinMode(LEFT_MOTOR_DRIVE_R_EN, OUTPUT);
  pinMode(LEFT_MOTOR_DRIVE_L_EN, OUTPUT);
  pinMode(LEFT_MOTOR_DRIVE_RPWM, OUTPUT);
  pinMode(LEFT_MOTOR_DRIVE_LPWM, OUTPUT);

  pinMode(LEFT_BACK_LIGHT_PIN, OUTPUT);
  pinMode(LEFT_FRONT_LIGHT_PIN, OUTPUT);
  pinMode(RIGHT_BACK_LIGHT_PIN, OUTPUT);
  pinMode(RIGHT_FRONT_LIGHT_PIN, OUTPUT);
  pinMode(LEFT_LONG_LIGHT_PIN, OUTPUT);
  pinMode(RIGHT_LONG_LIGHT_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(FRONT_CAMERA_SWITCH_PIN, OUTPUT);
  pinMode(BACK_CAMERA_SWITCH_PIN, OUTPUT);
  pinMode(TURRET_CAMERA_SWITCH_PIN, OUTPUT);
  pinMode(GUN_CAMERA_SWITCH_PIN, OUTPUT);
  pinMode(A_CAMERA_SWITCH_PIN, OUTPUT);
  pinMode(R_CAMERA_SWITCH_PIN, OUTPUT);

  pinMode(BATERY_VOLTAGE_PIN, INPUT);

  pinMode(STATUS_LED_PIN, OUTPUT);
}

#endif