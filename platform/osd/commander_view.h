#ifndef _COMMANDER_VIEW_H_
#define _COMMANDER_VIEW_H_

#include "max7456.h"

void set_commander_view(byte* data, MAX7456* osd) {
  uint8_t left_track_power = map(data[1], 0, 255, 0, 100);
  uint8_t right_track_power = map(data[2], 0, 255, 0, 100);

  bool left_track_up_direction = data[3] & 0x1;
  bool right_track_up_direction = data[3] & 0x2;
  uint8_t front_camera_position = data[3] >> 2 & 0xF;

  char buffer[3];
  sprintf(buffer, "%d", left_track_power);
  osd->print("LT:", 0, 13, false, false);
  osd->print(buffer, 3, 13, false, false);

  sprintf(buffer, "%d", right_track_power);
  osd->print("RT:", 0, 14, false, false);
  osd->print(buffer, 3, 14, false, false);

  osd->printMax7456Char(0x92, 11, 5, false, false);
  osd->printMax7456Char(0x93, 16, 5, false, false);
  osd->printMax7456Char(0x94, 11, 9, false, false);
  osd->printMax7456Char(0x95, 16, 9, false, false);

  osd->print("Com", 0, 0, false, false);

  osd->printMax7456Char(CAMERA_LEFT_CONSTRAIN, 7, 0, false, false);
  osd->printMax7456Char(CAMERA_POINTER, 8 + front_camera_position, 0, false, false);
  osd->printMax7456Char(CAMERA_RIGHT_CONSTRAIN, 19, 0, false, false);

  if (left_track_power > 0)
    osd->printMax7456Char(left_track_up_direction ? ARROW_UP_CHARACTER : ARROW_DOWN_CHARACTER, 6, 13, false, false);
  else
    osd->printMax7456Char(' ', 6, 13, false, false);
  if (right_track_power > 0)
    osd->printMax7456Char(right_track_up_direction ? ARROW_UP_CHARACTER : ARROW_DOWN_CHARACTER, 6, 14, false, false);
  else 
    osd->printMax7456Char(' ', 6, 14, false, false);
}

#endif