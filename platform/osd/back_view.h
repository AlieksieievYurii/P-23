#ifndef _BACK_VIEW_H_
#define _BACK_VIEW_H_

void set_back_view(byte* data, MAX7456* osd) {
  uint8_t left_track_power = map(data[1], 0, 255, 0, 100);
  uint8_t right_track_power = map(data[2], 0, 255, 0, 100);

  bool left_track_up_direction = data[3] & 0x1;
  bool right_track_up_direction = data[3] & 0x2;

  char buffer[3];
  sprintf(buffer, "%d", left_track_power);
  osd->print("LT:", 0, 13, false, false);
  osd->print(buffer, 3, 13, false, false);

  sprintf(buffer, "%d", right_track_power);
  osd->print("RT:", 0, 14, false, false);
  osd->print(buffer, 3, 14, false, false);

  osd->print("+", 13, 7, false, false);

  osd->print("Driver", 0, 0, false, false);

  if (left_track_power > 0)
    osd->printMax7456Char(left_track_up_direction ? ARROW_UP_CHARACTER : ARROW_DOWN_CHARACTER, 6, 13, false, false);
  else
    osd->printMax7456Char(' ', 6, 13, false, false);
  if (right_track_power > 0)
    osd->printMax7456Char(right_track_up_direction ? ARROW_UP_CHARACTER : ARROW_DOWN_CHARACTER, 6, 14, false, false);
  else
    osd->printMax7456Char(' ', 6, 14, false, false);

  osd->printMax7456Char(DRIVER_BACK_CAMERA, 26, 1, false, true);
}

#endif