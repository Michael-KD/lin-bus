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
LIN::Puppet puppet(PUPPET_ID, BAUD_RATE, DATA_LENGTH);

void setup() { 
  Serial.begin(19200); //for talking with the console
  pinMode(LIN_CS, OUTPUT);
  digitalWrite(LIN_CS, HIGH);
  if (MASTER_MODE) {
    master.startSerial(&Serial1);
    master.enable();
  } else {
    puppet.startSerial(&Serial1);
    puppet.enable();
  }
}

void loop() {
  if (MASTER_MODE) {
    Serial.println("");
    delay(5000);
    uint8_t data[DATA_LENGTH] = {0};
    Serial.println("Calling master.requestData()");
    bool success = master.requestData(data, PUPPET_ID);
    if (!success) {
      Serial.println("Send failed.");
    } else {
      Serial.println("Send balled.");
    }

    delay(5000);
    uint8_t transData[DATA_LENGTH] = {9, 7, 5, 3, 8, 6, 4, 2};
    bool transmitted = master.transmitData(transData);
    if (transmitted) {
      Serial.println("data transmitted");
    }
  } else if (!MASTER_MODE) {
    uint8_t data[DATA_LENGTH] = {0};

    int8_t busCheck = puppet.dataHasBeenRequested();
    if (busCheck) {
      if (busCheck == 1) {
        Serial.println("Data requested! Sending...");
        uint8_t data[DATA_LENGTH] = {0, 2, 4, 6, 7, 5, 3, 1};
        puppet.reply(data);
      } else if (busCheck == 2) {
        bool valid = puppet.readTransmittedData(data);
        if (valid) {
          Serial.println("Valid: ");
        } else {
          Serial.println("INVALID");
        }
        
        for (size_t i = 0; i < DATA_LENGTH; i++) {
          Serial.print(data[i], HEX);
          Serial.print(" ");
        }
        Serial.println("");
      }
    }
  }
}