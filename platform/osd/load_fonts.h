#ifndef _LOAD_FONTS_H_
#define _LOAD_FONTS_H_

#include "characters_table.h"

MAX7456 *osd = NULL;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  osd = new MAX7456(6);
  Serial.println(F("Initializing..."));
  Serial.println(F("Updating MAX7456 charset"));
  Serial.println(F("Aare you sure? (y/n)"));

  osd->activate_OSD(false);

  bool good_char = false;
  while (!good_char) {
    while (!Serial.available())
      ;
    char found;
    found = char(Serial.read());
    good_char = (found == 'y' || found == 'Y');
    if (found == 'n' || found == 'N') {
      Serial.println(F("------ NOTHING DONE ------"));
      Serial.println(F("please unplug your arduino"));
      while (true);
    }
  }

  unsigned char test[64] = {0};
 
  for (int i = 0; i <= 0xFF; i++) {
    Serial.print(F("Inserting "));
    Serial.print(i);
    Serial.println(F(" of 255"));
    get_character(TABLE_OF_CHARACTERS, i, test);
    osd->write_character(test, i);
  }

  Serial.println(F("Uploadig is done!"));
  Serial.println(F("Printing all characters"));

  osd->activate_OSD(true);

  for (int i = 0; i <= 0xff; i++) {
    osd->printMax7456Char(i, 1, 1, false);
    delay(2000);
  }
}

void loop() {}

#endif