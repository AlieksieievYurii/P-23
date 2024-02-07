#ifndef __OSD_H__
#define __OSD_H__

#include <Arduino.h>

class Osd {
public:
  Osd(HardwareSerial& osd_stream) {
    _osd_stream = &osd_stream;
  }

  void set_front_view(uint8_t left_track_power, uint8_t right_track_power, bool left_track_up_dir, bool right_track_up_dir, uint8_t camera_position) {
    uint8_t data = (camera_position << 2) | (right_track_up_dir << 1) | left_track_up_dir;
    byte package[5] = { 0x1, left_track_power, right_track_power, data, '\n' };
    _osd_stream->write(package, 5);
  }

  void set_back_view(uint8_t left_track_power, uint8_t right_track_power, bool left_track_up_dir, bool right_track_up_dir) {
    uint8_t data = (right_track_up_dir << 1) | left_track_up_dir;
    byte package[5] = { 0x2, left_track_power, right_track_power, data, '\n' };
    _osd_stream->write(package, 5);
  }

  void set_commander_view(uint8_t left_track_power, uint8_t right_track_power, bool left_track_up_dir, bool right_track_up_dir, uint8_t camera_position) {
     uint8_t data = (camera_position << 2) | (right_track_up_dir << 1) | left_track_up_dir;
    byte package[5] = { 0x3, left_track_power, right_track_power, data, '\n' };
    _osd_stream->write(package, 5);
  }

  void set_gunner_view(uint8_t left_track_power, uint8_t right_track_power, bool left_track_up_dir, bool right_track_up_dir, uint8_t loader_state, uint8_t gun_state) {
     uint8_t data = (loader_state << 4) | (gun_state << 2) | (right_track_up_dir << 1) | left_track_up_dir;
    byte package[5] = { 0x4, left_track_power, right_track_power, data, '\n' };
    _osd_stream->write(package, 5);
  }

private:
  HardwareSerial* _osd_stream;
};

#endif