#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 

#define OLED_RESET     -1 

class Display {
  public:
    Adafruit_SSD1306 display;
    void init() {
      display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
      display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
      display.clearDisplay();
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

    void test() {
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 0);
      display.println(F("scroll"));
      display.display();      // Show initial text
    }
};

#endif