#include "config.h"
#include "gate.h"

Gate gate;

void setup() {
  Serial.begin(9600);
  init_pins();

}

void loop() {
  if (Serial.available()) {
    char a = Serial.read();
    if (a == '1') {
      digitalWrite(FIRE_RELAY_PIN, HIGH);
      //gate.open();
    }

    if (a == '2') {
      digitalWrite(FIRE_RELAY_PIN, LOW);
      //gate.hold();
    }
    if (a == '3') {
      //gate.close();
    }
  }
  //gate.tick();
  //Serial.println(IS_IR_CUT);
  //delay(100);
}
