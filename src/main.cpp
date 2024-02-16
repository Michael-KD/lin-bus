#include <Arduino.h>

const uint32_t BAUD_RATE = 19200;

//pins
const uint8_t LIN_RXD = 0;
const uint8_t LIN_TXD = 1;
const uint8_t LIN_MCP_RESET = 2;
const uint8_t LIN_CS = 3;

//ids (no particular order lol)
const uint8_t CONTROLLER_ID = 0x3a;
const uint8_t NODE_ID = 0x3b;

void setup() {
  Serial.begin(9600);
  pinMode(LIN_CS, OUTPUT);
  digitalWrite(LIN_CS, HIGH);
}

void loop() {
  
}
