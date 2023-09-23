#ifndef _MAIN_H_
#define _MAIN_H_

#include "front_view.h"
#include "back_view.h"

//#define ARDUINO_CONSOLE

MAX7456 *osd = NULL;

const int PACKAGE_SIZE = 10;
char package_buffer[PACKAGE_SIZE] = { 0 };

void setup() {
  Serial.begin(9600);
  SPI.begin();
  osd = new MAX7456(6);
  osd->set_display_offsets(40, 30);
  osd->set_blink_params(_8fields, _3BT_BT);
  osd->activate_OSD(true);
  osd->print("START", 5, 5, false, false);
}

void loop() {
  if (Serial.available() > 0) {

    int rlen = Serial.readBytes(package_buffer, PACKAGE_SIZE);

#ifdef ARDUINO_CONSOLE
    uint8_t package_id = package_buffer[0] - '0';
#else
    uint8_t package_id = package_buffer[0];
#endif
    switch (package_id) {
      case 0x1:
        osd->clear();
        set_front_view(package_buffer, osd);
        break;
      case 0x2:
        osd->clear();
        set_back_view(package_buffer, osd);
        break;
    }
  }
}
#endif