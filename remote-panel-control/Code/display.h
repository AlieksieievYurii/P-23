#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define REFRESH_TIME 2000

#define WINDOW_CONFIG_ID 3
#define WINDOW_COMMANDS_ID 1
#define WINDOW_OVERVIEW 0

#define OLED_RESET -1

#define MAX(VALUE, CONSTRAIN) (((VALUE) > (CONSTRAIN)) ? (CONSTRAIN) : (VALUE))


#define CONFIG_VIEW_MAX_PRINTING_ITEMS 4
#define CONFIG_VIEW_ITEM_PRINTING_POSITION_START 29


#define COMMANDS_VIEW_MAX_PRINTING_ITEMS 4
#define COMMANDS_VIEW_ITEM_PRINTING_POSITION_START 29

#define RESET_VALUES \
  _config_view_window_item_point = 0; \
  _config_view_window_item_hower = 0; \
  _config_view_window_item_print_step = 0; \
  _commands_view_window_item_point = 0; \
  _commands_view_window_item_hower = 0; \
  _commands_view_window_item_print_step = 0;

enum ModeSelection {
  VIEW,
  MENU,
  ITEM
};

class Display {
public:
  bool connected = false;
  float battery_voltage = 0;
  uint8_t battery_indicator = 0;
  uint32_t ping = 0;

  void init(ConfigHolder& config_holder) {
    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
    display.clearDisplay();

    _config_holder = &config_holder;
  }

  void showSplash() {
    display.setTextSize(1.5);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(40, 28);
    display.println(F("Loading..."));
    display.display();
    delay(2000);
    display.clearDisplay();
  }

  void next_menu_item() {
    if (_mode == MENU) {

      if (_view_window_selected == WINDOW_CONFIG_ID)
        _next_item_in_config_view();
      else if (_view_window_selected == WINDOW_COMMANDS_ID)
        _next_item_in_commands_view();

    } else if (_mode == VIEW) {

      if (_view_window_selected < 3)
        _view_window_selected++;
      else
        _view_window_selected = 0;

    } else if (_mode == ITEM) {

      if (_view_window_selected == WINDOW_CONFIG_ID) {
        if (_config_view_item_temp_selection < _config_holder->config_view_items[_config_view_window_item_point].values_number - 1)
          _config_view_item_temp_selection++;
        else
          _config_view_item_temp_selection = 0;
      }
    }
    draw();
  }

  void previous_menu_item() {
    if (_mode == MENU) {

      if (_view_window_selected == WINDOW_CONFIG_ID)
        _previous_item_in_config_view();
      else if (_view_window_selected == WINDOW_COMMANDS_ID)
        _previous_item_in_commands_view();

    } else if (_mode == VIEW) {

      if (_view_window_selected > 0)
        _view_window_selected--;
      else
        _view_window_selected = 3;

    } else if (_mode == ITEM) {

      if (_view_window_selected == WINDOW_CONFIG_ID) {
        if (_config_view_item_temp_selection > 0)
          _config_view_item_temp_selection--;
        else
          _config_view_item_temp_selection = _config_holder->config_view_items[_config_view_window_item_point].values_number - 1;
      }
    }

    draw();
  }

  void select() {
    if (_mode == VIEW) {
      RESET_VALUES;
      _mode = MENU;
    } else if (_mode == MENU) {

      if (_view_window_selected == WINDOW_CONFIG_ID) {
        _mode = ITEM;
        _config_view_item_temp_selection = _config_holder->config_view_items[_config_view_window_item_point].selected_item;
      } else if (_view_window_selected == WINDOW_COMMANDS_ID) {
        _config_holder->send_command(_config_holder->command_items[_commands_view_window_item_point].id);
      }

    } else if (_mode == ITEM) {
      _config_holder->config_view_items[_config_view_window_item_point].selected_item = _config_view_item_temp_selection;
      _config_view_item_temp_selection = 0;
      _mode = MENU;
    }

    draw();
  }

  void back() {
    if (_mode == ITEM) {
      _config_view_item_temp_selection = 0;
      _mode = MENU;
    } else if (_mode == MENU) {
      RESET_VALUES;
      _mode = VIEW;
    }

    draw();
  }

  void tick() {
    if (millis() - _last_update >= REFRESH_TIME) {
      draw();
      _last_update = millis();
    }

    // Update the output config variables
    _config_holder->communication_channel = _config_holder->config_view_items[0].selected_item + 1;  // Because the first(zero) item in the array means first channel
    _config_holder->back_light_mode = _config_holder->config_view_items[1].selected_item;
    _config_holder->front_light_mode = _config_holder->config_view_items[2].selected_item;
    _config_holder->long_light_mode = _config_holder->config_view_items[3].selected_item;
    _config_holder->turret_light_mode = _config_holder->config_view_items[4].selected_item;
    // -----------------------------------
  }


private:
  Adafruit_SSD1306 display;
  uint32_t _last_update = 0;

  ModeSelection _mode = VIEW;

  uint8_t _view_window_selected = 0;

  uint8_t _config_view_window_item_hower = 0;
  uint8_t _config_view_window_item_point = 0;
  uint8_t _config_view_window_item_print_step = 0;
  uint8_t _config_view_item_temp_selection = 0;

  uint8_t _commands_view_window_item_hower = 0;
  uint8_t _commands_view_window_item_point = 0;
  uint8_t _commands_view_window_item_print_step = 0;

  ConfigHolder* _config_holder;

  void draw() {
    display.clearDisplay();

    // Print Menu Item Selection
    for (uint8_t i = 0; i < 4; i++) {
      if (i == _view_window_selected)
        display.fillRect(i * 32, 16, 32, 3, SSD1306_WHITE);
      else
        display.drawRect(i * 32, 16, 32, 3, SSD1306_WHITE);
      //-------------------------------
    }
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    if (connected) {
      // Printing Tank Battery Voltage & Connection Ping
      display.setCursor(22, 5);
      display.println(battery_voltage);
      display.setCursor(55, 5);
      display.println('V');
      display.setCursor(85, 5);
      display.print(F("P:"));
      display.setCursor(95, 5);
      display.print(ping);
      // -----------------------------------------------

      // Drawing battery indicator
      uint8_t indicator = MAX(battery_indicator, 4);
      display.drawLine(0, 0, 0, 14, SSD1306_WHITE);
      display.drawLine(0, 14, 18, 14, SSD1306_WHITE);
      display.drawLine(18, 14, 18, 10, SSD1306_WHITE);
      display.drawLine(18, 10, 20, 10, SSD1306_WHITE);
      display.drawLine(20, 10, 20, 4, SSD1306_WHITE);
      display.drawLine(20, 4, 18, 4, SSD1306_WHITE);
      display.drawLine(18, 4, 18, 0, SSD1306_WHITE);
      display.drawLine(18, 0, 0, 0, SSD1306_WHITE);
      for (uint8_t i = 1; i <= indicator; i++)
        display.fillRect((i * 2) + (2 * (i - 1)), 2, 3, 11, SSD1306_WHITE);
      // -------------------------------------------------
    } else {
      display.setCursor(27, 5);
      display.print("Disconected!");
    }

    if (_view_window_selected == WINDOW_CONFIG_ID) {
      _print_config_view();
    } else if (_view_window_selected == WINDOW_COMMANDS_ID) {
      _print_commands_view();
    } else if (_view_window_selected == WINDOW_OVERVIEW) {
      _print_overview_view();
    }

    display.display();
  }

  void _print_config_view() {
    display.setTextSize(1);
    display.setCursor(0, 21);
    display.setTextColor(SSD1306_WHITE);
    display.println("> Config");


    for (uint8_t i = 0; i < CONFIG_VIEW_MAX_PRINTING_ITEMS; i++) {
      if ((_mode == MENU || _mode == ITEM) && _config_view_window_item_hower == i)
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      else
        display.setTextColor(SSD1306_WHITE);

      display.setCursor(0, CONFIG_VIEW_ITEM_PRINTING_POSITION_START + i * 9);
      uint8_t config_item_position = i + _config_view_window_item_print_step;
      uint8_t printing_value_position = strlen(_config_holder->config_view_items[config_item_position].name) * 6;
      display.println(_config_holder->config_view_items[config_item_position].name);
      display.setCursor(printing_value_position, CONFIG_VIEW_ITEM_PRINTING_POSITION_START + i * 9);
      display.println(":");

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(printing_value_position + 18, CONFIG_VIEW_ITEM_PRINTING_POSITION_START + i * 9);
      if (_mode == ITEM && _config_view_window_item_point == config_item_position) {
        char* temp_selected_value = _config_holder->config_view_items[config_item_position].values[_config_view_item_temp_selection];
        display.drawRect(printing_value_position + 15, CONFIG_VIEW_ITEM_PRINTING_POSITION_START + i * 9 - 1, strlen(temp_selected_value) * 8, 9, SSD1306_INVERSE);
        display.println(temp_selected_value);
      } else {
        display.println(_config_holder->config_view_items[config_item_position].values[_config_holder->config_view_items[config_item_position].selected_item]);
      }
    }
  }

  void _previous_item_in_config_view() {
    if (_config_view_window_item_point > 0) {
      _config_view_window_item_point--;
      if (_config_view_window_item_hower > 0)
        _config_view_window_item_hower--;
      else
        _config_view_window_item_print_step--;
    } else {
      _config_view_window_item_point = CONFIG_VIEW_ITEM_COUNTS - 1;
      _config_view_window_item_hower = CONFIG_VIEW_MAX_PRINTING_ITEMS - 1;
      _config_view_window_item_print_step = CONFIG_VIEW_ITEM_COUNTS - CONFIG_VIEW_MAX_PRINTING_ITEMS;
    }
  }

  void _next_item_in_config_view() {
    if (++_config_view_window_item_point >= CONFIG_VIEW_ITEM_COUNTS) {
      _config_view_window_item_point = 0;
      _config_view_window_item_hower = 0;
      _config_view_window_item_print_step = 0;
    } else {
      if (_config_view_window_item_hower < CONFIG_VIEW_MAX_PRINTING_ITEMS - 1)
        _config_view_window_item_hower++;
      else
        _config_view_window_item_print_step++;
    }
  }

  void _next_item_in_commands_view() {
    if (++_commands_view_window_item_point >= COMMAND_ITEMS_COUNTS) {
      _commands_view_window_item_point = 0;
      _commands_view_window_item_hower = 0;
      _commands_view_window_item_print_step = 0;
    } else {
      if (_commands_view_window_item_hower < COMMANDS_VIEW_MAX_PRINTING_ITEMS - 1)
        _commands_view_window_item_hower++;
      else
        _commands_view_window_item_print_step++;
    }
  }

  void _previous_item_in_commands_view() {
    if (_commands_view_window_item_point > 0) {
      _commands_view_window_item_point--;
      if (_commands_view_window_item_hower > 0)
        _commands_view_window_item_hower--;
      else
        _commands_view_window_item_print_step--;
    } else {
      _commands_view_window_item_point = COMMAND_ITEMS_COUNTS - 1;
      _commands_view_window_item_hower = COMMANDS_VIEW_MAX_PRINTING_ITEMS - 1;
      _commands_view_window_item_print_step = COMMAND_ITEMS_COUNTS - COMMANDS_VIEW_MAX_PRINTING_ITEMS;
    }
  }

  void _print_commands_view() {
    display.setTextSize(1);
    display.setCursor(0, 21);
    display.setTextColor(SSD1306_WHITE);
    display.println("$ Commands");

    for (uint8_t i = 0; i < COMMANDS_VIEW_MAX_PRINTING_ITEMS; i++) {
      if ((_mode == MENU || _mode == ITEM) && _commands_view_window_item_hower == i)
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      else
        display.setTextColor(SSD1306_WHITE);

      display.setCursor(0, COMMANDS_VIEW_ITEM_PRINTING_POSITION_START + i * 9);
      uint8_t config_item_position = i + _commands_view_window_item_print_step;
      display.println(_config_holder->command_items[config_item_position].name);
    }
    display.setTextColor(SSD1306_WHITE);
    display.drawLine(70, 21, 70, 40, SSD1306_WHITE);
    display.drawLine(70, 40, 140, 40, SSD1306_WHITE);
    display.setCursor(72, 21);
    display.println("State:");
    display.setCursor(72, 30);
    if (_config_holder->last_error_code != 0)
      display.println(repr_CE_status(CEStatus::FAILED));
    else
      display.println(repr_CE_status(_config_holder->command_execution_status));
  }

  void _print_overview_view(void) {
    display.setTextSize(1);
    display.setCursor(0, 21);
    display.setTextColor(SSD1306_WHITE);
    display.println("Loader:");
    display.setCursor(45, 21);
    display.println(repr_loader_state(_config_holder->loader_state));
    display.setCursor(0, 30);
    display.println("Task status:");
    display.setCursor(75, 30);
    display.println(repr_CE_status(_config_holder->command_execution_status));
    display.setCursor(0, 39);
    display.println("Error:");
    display.setCursor(45, 39);
    display.println(_config_holder->last_error_code);
    display.setCursor(0, 48);
    display.println(repr_CE_error(_config_holder->last_error_code));
  }
};

Display DisplayInstance;

#endif