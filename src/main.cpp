#include <Arduino.h>
#include "lin.h"

const uint32_t BAUD_RATE = 19200;
const size_t DATA_LENGTH = 8;

//pins
const uint8_t LIN_RXD = 0;
const uint8_t LIN_TXD = 1;
const uint8_t LIN_MCP_RESET = 2;
const uint8_t LIN_CS = 3;

//ids
const uint8_t PUPPET_ID = 0x3b; //completely arbitrary

const bool MASTER_MODE = true; //change to swap between puppet/master for testing

LIN::Master master(BAUD_RATE, DATA_LENGTH);
// LIN::Puppet puppet(&Serial1, PUPPET_ID, BAUD_RATE, DATA_LENGTH);

void setup() {
  Serial.begin(19200); //for talking with the console
  master.startSerial(&Serial1);
  // pinMode(LIN_CS, OUTPUT);
  // digitalWrite(LIN_CS, HIGH);
  if (MASTER_MODE)
    master.enable();
  // else
  //   puppet.enable();
}

void loop() {

  // Serial.println("HELLO PEOPLE");
  delay(5000);

  if (MASTER_MODE) {
    
    uint8_t data[DATA_LENGTH] = {0};
    Serial.println("Calling master.requestData()");
    bool success = master.requestData(data, PUPPET_ID);
    if (!success) {
      Serial.println("Send failed.");
    }
    delay(1000);
  } else {
    
  }
}