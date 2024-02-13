#include <Arduino.h>
#include "lin_bus.h"

const int BAUD_RATE = 19200;


void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("hello world");
}
