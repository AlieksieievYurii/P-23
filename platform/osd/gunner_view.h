#ifndef _GUNNER_VIEW_H_
#define _GUNNER_VIEW_H_

#include "max7456.h"

#define LS_REQUIRES_CALIBRATION 0
#define LS_CALIBRATION 1
#define LS_LOADING 2
#define LS_UNLOADING 3
#define LS_LOADED 4
#define LS_UNLOADED 5
#define LS_RELOADING 6
#define LS_FAILURE 7

#define GUN_STATE_NONE 0
#define GUN_STATE_NOT_READY 1
#define GUN_STATE_READY 2

void set_gunner_view(byte* data, MAX7456* osd) {
  uint8_t left_track_power = map(data[1], 0, 255, 0, 100);
  uint8_t right_track_power = map(data[2], 0, 255, 0, 100);

  bool left_track_up_direction = data[3] & 0x1;
  bool right_track_up_direction = data[3] & 0x2;
  uint8_t state = data[3] >> 4;
  uint8_t gun_state = (data[3] >> 2) & 0x3;

  char buffer[3];
  sprintf(buffer, "%d", left_track_power);
  osd->print("LT:", 0, 13, false, false);
  osd->print(buffer, 3, 13, false, false);

  sprintf(buffer, "%d", right_track_power);
  osd->print("RT:", 0, 14, false, false);
  osd->print(buffer, 3, 14, false, false);

  osd->printMax7456Char(0x96, 13, 7, false, false);

  osd->print("Gun", 0, 0, false, false);

  switch (state) {
    case LS_REQUIRES_CALIBRATION:
      osd->print("REQ CAL", 11, 0, true, false);
      break;
    case LS_CALIBRATION:
      osd->print("CALIBRATING", 9, 0, false, false);
      break;
    case LS_LOADING:
      osd->print("LOADING", 10, 0, false, false);
      break;
    case LS_UNLOADING:
      osd->print("UNLOADING", 10, 0, false, false);
      break;
    case LS_LOADED:
      osd->print("LOADED", 11, 0, false, false);
      break;
    case LS_UNLOADED:
      osd->print("UNLOADED", 9, 0, false, false);
      break;
    case LS_RELOADING:
      osd->print("RELOADING", 9, 0, false, false);
      break;
    case LS_FAILURE:
      osd->print("ERROR", 12, 0, false, true);
      break;
  }

  switch(gun_state) {
    case GUN_STATE_NONE:
      osd->print("-----", 11, 14, false, false);
      break;
    case GUN_STATE_NOT_READY:
      osd->print("NOT READY", 9, 14, false, false);
      break;
    case GUN_STATE_READY:
      osd->print("READY", 11, 14, false, false);
      break;
  }

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