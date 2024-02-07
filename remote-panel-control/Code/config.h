#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CHIP_ENABLE_IN 49
#define CHIP_SELECT_IN 48

unsigned char ENCRYPTION_KEY[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

#define IS_SWITCH_ON(PIN) !digitalRead(PIN)
#define READ_ANALOG_VALUE(pin) map(analogRead(pin), 0, 1023, 0, 255)

#define IN_RANGE(x, A, B) A <= x &&x <= B

#define LIGHT_SWITCH_BACK 43
#define LIGHT_SWITCH_FRONT 44
#define LIGHT_SWITCH_LONG 45
#define LIGHT_SWITCH_TURRET 46

#define GUN_SWITCH 30
#define RELOADER_TAKE_BUTTON 31
#define RELOADER_LOAD_BUTTON 32
#define GUN_STABILIZER_SWITCH 33
#define FIRE_A_BUTTON 34

#define BUZZER 3

#define ARM_TURRET_JOY_X A0
#define ARM_TURRET_JOY_Y A1
#define ARM_TURRET_JOY_R A2

#define DRIVE_JOY_X A3
#define DRIVE_JOY_Y A4
#define DRIVE_JOY_R A5

#define FIRE_B_BUTTON 36
#define FOLLOW_TARGET_BUTTON 37
#define FOLLOW_TARGET_LED 38

#define DRIVING_SPEED_POTENTIOMETER 10
#define DRIVING_MODE_SWITCH 42
#define DRIVING_ERROR_LED 41

#define DISPLAY_LEFT_BUTTON 24
#define DISPLAY_OK_CANCEL_BUTTON 23
#define DISPLAY_RIGHT_BUTTON 22

#define ARM_MODE_SWITCH 29
#define BATTLE_MODE_DOUBLE_SWITCH A13

#define CONNECTION_STATIUS_LED_GREEN_PIN 13
#define CONNECTION_STATIUS_LED_RED_PIN 2

#define AUTO_SHOOTING_SWITCH 12
#define FIRE_SWITCH 11
#define KEY_SWITCH 10
#define FIRE_CON_GREEN_LED 8
#define FIRE_CON_RED_LED 9

#define ARM_TURRET_SPEED_POTENTIOMETER A8
#define PROG_POTENTIOMETER A9
#define ARM_TURRET_ERROR_LED 40

#define LOADER_TAKE_GREEN_LED 4
#define LOADER_TAKE_RED_LED 5
#define LOADER_LOAD_GREEN_LED 6
#define LOADER_LOAD_RED_LED 7

#define CAMERA_JOY_X A6
#define CAMERA_JOY_Y A7
#define CAMERA_JOY_BUTTON 39

#define CAMERA_SWITCH A11
#define CAMERA_MOVEMENT_SPEED_POTENTIOMETER A12
#define LASER_SWITCH 35

#define DISPLAY_ADDRESS 0x3C

#define CONFIG_VIEW_ITEM_COUNTS 9
#define COMMAND_ITEMS_COUNTS 13

#define DISCONNECTION_TIMEOUT 5000
#define ORANGE_PING_DELAY 100
#define RED_PING_DELAY 200

void init_pins() {
  pinMode(LIGHT_SWITCH_BACK, INPUT_PULLUP);
  pinMode(LIGHT_SWITCH_FRONT, INPUT_PULLUP);
  pinMode(LIGHT_SWITCH_LONG, INPUT_PULLUP);
  pinMode(LIGHT_SWITCH_TURRET, INPUT_PULLUP);

  pinMode(GUN_SWITCH, INPUT_PULLUP);
  pinMode(RELOADER_TAKE_BUTTON, INPUT_PULLUP);
  pinMode(RELOADER_LOAD_BUTTON, INPUT_PULLUP);
  pinMode(GUN_STABILIZER_SWITCH, INPUT_PULLUP);
  pinMode(FIRE_A_BUTTON, INPUT_PULLUP);

  pinMode(ARM_TURRET_JOY_X, INPUT);
  pinMode(ARM_TURRET_JOY_Y, INPUT);
  pinMode(ARM_TURRET_JOY_R, INPUT);

  pinMode(DRIVE_JOY_X, INPUT);
  pinMode(DRIVE_JOY_Y, INPUT);
  pinMode(DRIVE_JOY_R, INPUT);

  pinMode(FOLLOW_TARGET_BUTTON, INPUT_PULLUP);
  pinMode(FIRE_B_BUTTON, INPUT_PULLUP);
  pinMode(FOLLOW_TARGET_LED, OUTPUT);

  pinMode(DRIVING_SPEED_POTENTIOMETER, INPUT);
  pinMode(DRIVING_MODE_SWITCH, INPUT_PULLUP);
  pinMode(DRIVING_ERROR_LED, OUTPUT);

  pinMode(DISPLAY_LEFT_BUTTON, INPUT_PULLUP);
  pinMode(DISPLAY_OK_CANCEL_BUTTON, INPUT_PULLUP);
  pinMode(DISPLAY_RIGHT_BUTTON, INPUT_PULLUP);

  pinMode(ARM_MODE_SWITCH, INPUT);
  pinMode(BATTLE_MODE_DOUBLE_SWITCH, INPUT);

  pinMode(AUTO_SHOOTING_SWITCH, INPUT_PULLUP);
  pinMode(FIRE_SWITCH, INPUT_PULLUP);
  pinMode(KEY_SWITCH, INPUT_PULLUP);
  pinMode(FIRE_CON_GREEN_LED, OUTPUT);
  pinMode(FIRE_CON_RED_LED, OUTPUT);

  pinMode(ARM_TURRET_SPEED_POTENTIOMETER, INPUT);
  pinMode(PROG_POTENTIOMETER, INPUT);
  pinMode(ARM_TURRET_ERROR_LED, OUTPUT);

  pinMode(LOADER_TAKE_GREEN_LED, OUTPUT);
  pinMode(LOADER_TAKE_RED_LED, OUTPUT);
  pinMode(LOADER_LOAD_GREEN_LED, OUTPUT);
  pinMode(LOADER_LOAD_RED_LED, OUTPUT);

  pinMode(CAMERA_JOY_X, INPUT);
  pinMode(CAMERA_JOY_Y, INPUT);
  pinMode(CAMERA_JOY_BUTTON, INPUT);

  pinMode(CAMERA_SWITCH, INPUT);
  pinMode(CAMERA_MOVEMENT_SPEED_POTENTIOMETER, INPUT);
  pinMode(LASER_SWITCH, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);
  pinMode(CONNECTION_STATIUS_LED_GREEN_PIN, OUTPUT);
  pinMode(CONNECTION_STATIUS_LED_RED_PIN, OUTPUT);
}

struct ConfigItem {
  char name[20];
  uint8_t values_number = 3;  //max 5
  uint8_t selected_item = 0;
  char values[5][5] = { 0 };
};

struct CommandItem {
  char name[7];
  uint8_t id;
};


class ConfigHolder {
public:
  struct ConfigItem *config_view_items;
  struct CommandItem *command_items;

  uint8_t back_light_mode = 0;    // 0 - standalone, 1 - blinking, 2 - fast blinking
  uint8_t front_light_mode = 0;   // 0 - standalone, 1 - blinking, 2 - fast blinking
  uint8_t long_light_mode = 0;    // 0 - standalone, 1 - blinking, 2 - fast blinking
  uint8_t turret_light_mode = 0;  // 0 - standalone, 1 - blinking, 2 - fast blinking

  uint8_t communication_channel = 1;

  CEStatus command_execution_status = CEStatus::IDLE;
  uint8_t last_error_code = 0;
  uint8_t loader_state = 0;

  ConfigHolder() {

    struct ConfigItem comm_channel;
    comm_channel.values_number = 5;
    strcpy(comm_channel.name, "Com Channel");
    strcpy(comm_channel.values[0], "1");
    strcpy(comm_channel.values[1], "2");
    strcpy(comm_channel.values[2], "3");
    strcpy(comm_channel.values[3], "4");
    strcpy(comm_channel.values[4], "5");

    struct ConfigItem front_light;
    strcpy(front_light.name, "Front Lights");
    strcpy(front_light.values[0], "S");
    strcpy(front_light.values[1], "BL");
    strcpy(front_light.values[2], "FBL");

    struct ConfigItem back_light;
    strcpy(back_light.name, "Back Lights");
    strcpy(back_light.values[0], "S");
    strcpy(back_light.values[1], "BL");
    strcpy(back_light.values[2], "FBL");

    struct ConfigItem long_light;
    strcpy(long_light.name, "Long Lights");
    strcpy(long_light.values[0], "S");
    strcpy(long_light.values[1], "BL");
    strcpy(long_light.values[2], "FBL");

    struct ConfigItem turret_light;
    strcpy(turret_light.name, "Turret Lights");
    strcpy(turret_light.values[0], "S");
    strcpy(turret_light.values[1], "BL");
    strcpy(turret_light.values[2], "FBL");

    struct ConfigItem c;
    strcpy(c.name, "C");
    strcpy(c.values[0], "4");
    strcpy(c.values[1], "BL");
    strcpy(c.values[2], "FBL");

    struct ConfigItem d;
    strcpy(d.name, "D");
    strcpy(d.values[0], "3");
    strcpy(d.values[1], "BL");
    strcpy(d.values[2], "FBL");

    struct ConfigItem e;
    strcpy(e.name, "E");
    strcpy(e.values[0], "2");
    strcpy(e.values[1], "BL");
    strcpy(e.values[2], "FFFFFF");

    struct ConfigItem f;
    strcpy(f.name, "f");
    strcpy(f.values[0], "1");
    strcpy(f.values[1], "BL");
    strcpy(f.values[2], "A");

    struct CommandItem ci_calibrate;
    strcpy(ci_calibrate.name, "Cal");
    ci_calibrate.id = 0x04;

    struct CommandItem ci_connect_loader;
    strcpy(ci_connect_loader.name, "CL");
    ci_connect_loader.id = 0x07;

    struct CommandItem ci_disconnect_loader;
    strcpy(ci_disconnect_loader.name, "DCL");
    ci_disconnect_loader.id = 0x08;

    struct CommandItem ci_falcify_calibration;
    strcpy(ci_falcify_calibration.name, "FC");
    ci_falcify_calibration.id = 0x11;

    struct CommandItem ci_open_loading_drum_gate;
    strcpy(ci_open_loading_drum_gate.name, "OLDG");
    ci_open_loading_drum_gate.id = 0x05;

    struct CommandItem ci_close_loading_drum_gate;
    strcpy(ci_close_loading_drum_gate.name, "CLDG");
    ci_close_loading_drum_gate.id = 0x06;

    struct CommandItem ci_rotate_loading_drum_clockwise;
    strcpy(ci_rotate_loading_drum_clockwise.name, "RLDC");
    ci_rotate_loading_drum_clockwise.id = 0x09;

    struct CommandItem ci_rotate_loading_drum_counter_clockwise;
    strcpy(ci_rotate_loading_drum_counter_clockwise.name, "RLDCC");
    ci_rotate_loading_drum_counter_clockwise.id = 0x0A;

    struct CommandItem ci_push_feeder;
    strcpy(ci_push_feeder.name, "PUSHF");
    ci_push_feeder.id = 0x0B;

    struct CommandItem ci_pull_feeder;
    strcpy(ci_pull_feeder.name, "PULLF");
    ci_pull_feeder.id = 0x0C;

    struct CommandItem ci_open_breechblock_gate;
    strcpy(ci_open_breechblock_gate.name, "OBBG");
    ci_open_breechblock_gate.id = 0x0D;

    struct CommandItem ci_close_breechblock_gate;
    strcpy(ci_close_breechblock_gate.name, "CBBG");
    ci_close_breechblock_gate.id = 0x0E;

    struct CommandItem ci_pre_close_breechblock_gate;
    strcpy(ci_pre_close_breechblock_gate.name, "PCBBG");
    ci_pre_close_breechblock_gate.id = 0x0F;

    static struct ConfigItem buffer[CONFIG_VIEW_ITEM_COUNTS] = { comm_channel, back_light, front_light, long_light, turret_light, c, d, e, f };
    static struct CommandItem command_items_buff[COMMAND_ITEMS_COUNTS] = {
      ci_calibrate,
      ci_falcify_calibration,
      ci_connect_loader,
      ci_disconnect_loader,
      ci_open_loading_drum_gate,
      ci_close_loading_drum_gate,
      ci_rotate_loading_drum_clockwise,
      ci_rotate_loading_drum_counter_clockwise,
      ci_push_feeder,
      ci_pull_feeder,
      ci_open_breechblock_gate,
      ci_close_breechblock_gate,
      ci_pre_close_breechblock_gate
    };
    config_view_items = buffer;
    command_items = command_items_buff;
  }
  bool is_communication_channel_changed() {
    if (communication_channel != _last_communication_chanel_value) {
      _last_communication_chanel_value = communication_channel;
      return true;
    }
    return false;
  }

  uint8_t get_command() {
    uint8_t buf = _command_to_execute;
    _command_to_execute = 0;
    return buf;
  }

  void send_command(const uint8_t command) {
    _command_to_execute = command;
  }
private:
  uint8_t _last_communication_chanel_value = 1;
  uint8_t _command_to_execute = 0;
};

ConfigHolder ConfigHolderInstance;

#endif