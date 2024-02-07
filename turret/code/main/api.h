#ifndef __API_H__
#define __API_H__

#define INCOMING_MESSAGE_SIZE 6  // Note it does not include Start-of-Frame delimiter and check-sum
#define OUTCOMING_MESSAGE_SIZE 4

const uint8_t get_check_sum(const uint8_t* data, const uint8_t message_size) {
  uint16_t sum = 0;
  for (uint8_t i = 1; i <= message_size; i++)
    sum += data[i];
  return sum / message_size;
}


bool read_serial_message(uint8_t* data) {
  if (Serial.available() != INCOMING_MESSAGE_SIZE + 2)
    return false;

  uint32_t last_update_time = millis();
  uint8_t index = 1;
  bool start = false;

  while (true) {
    if (millis() - last_update_time > 50)
      return false;

    if (Serial.available() == 0)
      continue;

    uint8_t incoming_byte = Serial.read();

    if (start) {
      data[index++] = incoming_byte;
      if (index == INCOMING_MESSAGE_SIZE + 2) {

        // Check the checksum which is the latest item
        if (data[INCOMING_MESSAGE_SIZE + 1] == get_check_sum(data, INCOMING_MESSAGE_SIZE)) {
          return true;
        } else {
          index = 1;
          start = false;
        }
      }
    }

    if (start == false && incoming_byte == 0xFF) {
      start = true;
      data[0] = 0xFF;
    }
  }
}

void send_outcoming_data(const uint8_t current_command, const uint8_t status, const uint8_t error_code, const uint8_t loader_state, const bool shell_connected) {
  static uint8_t prev_current_command = 0;
  static uint8_t prev_loader_state = 0;
  static uint8_t prev_status = 0;
  static uint8_t prev_error_code = 0;

  if (current_command == prev_current_command && loader_state == prev_loader_state && prev_status == status && prev_error_code == error_code)
    return;

  uint8_t outcoming_data[OUTCOMING_MESSAGE_SIZE + 2] = { 0 };

  outcoming_data[0x0] = 0xFF;
  outcoming_data[0x1] = current_command;
  outcoming_data[0x2] = status;
  outcoming_data[0x3] = (shell_connected << 3) | loader_state;
  outcoming_data[0x4] = error_code;
  outcoming_data[0x5] = get_check_sum(outcoming_data, OUTCOMING_MESSAGE_SIZE);
  Serial.write(outcoming_data, OUTCOMING_MESSAGE_SIZE + 2);
  prev_current_command = current_command;
  prev_loader_state = loader_state;
  prev_status = status;
  prev_error_code = error_code;
}

#endif