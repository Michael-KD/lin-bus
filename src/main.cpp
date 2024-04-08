#include <Arduino.h>
#include "lin.h"

const uint32_t BAUD_RATE = 19200;

//pins
const uint8_t LIN_RXD = 0;
const uint8_t LIN_TXD = 1;
const uint8_t LIN_MCP_RESET = 2;
const uint8_t LIN_CS = 3;

//ids 
const uint8_t PUPPET_ID = 0x3b; //completely arbitrary

const bool MASTER_MODE = true; //change to swap between puppet/master for testing

size_t dataLength = 1;

LIN::Master* master;
LIN::Puppet* puppet;

uint8_t* testData;

const size_t TRIALS = 12;
const size_t ITERS = 30;

size_t trial = 0;
size_t iter = 0;

uint64_t** timeData;

bool printedData = false;

void setup() {
  Serial.begin(19200); //for talking with the console
  pinMode(LIN_CS, OUTPUT);
  digitalWrite(LIN_CS, HIGH);
  master = new LIN::Master(BAUD_RATE, dataLength);
  puppet = new LIN::Puppet(PUPPET_ID, BAUD_RATE, dataLength);
  if (MASTER_MODE) {
    master->startSerial(&Serial1);
    master->enable();
  } else {
    puppet->startSerial(&Serial1);
    puppet->enable();
  }

  if (MASTER_MODE) {
    timeData = new uint64_t*[TRIALS];
    for (size_t i = 0; i < TRIALS; i++) {
      timeData[i] = new uint64_t[ITERS];
    }
  }

}

void loop() {
  if (MASTER_MODE) {
    if (trial < TRIALS) {
      uint8_t data[dataLength] = {0};
      uint64_t startMicros = micros();
      bool success = master->requestData(data, PUPPET_ID);
      uint64_t endMicros = micros();
      if (success) {
        timeData[trial][iter] = endMicros - startMicros;
      } else {
        timeData[trial][iter] = 5000000; //timeout
      }

      if (iter == ITERS) {
        delete master;
        dataLength *= 2;
        master = new LIN::Master(BAUD_RATE, dataLength);
        delay(100); //give puppet time to clean
        iter = 0;
        trial++;
      }
    } else if (!printedData) {
      int bytes = 1;
      Serial.println("Bytes");
      for (size_t i = 0; i < TRIALS; i++) {
        Serial.print(bytes + ",");
        bytes *= 2;
        for (size_t j = 0; j < ITERS; j++) {
          Serial.print(timeData[i][j] + ",");
        }
        Serial.println();
      }
      printedData = true;
    }
  } else if (!MASTER_MODE) {
    if (trial < TRIALS) {
      puppet = new LIN::Puppet(PUPPET_ID, BAUD_RATE, dataLength);
      testData = new uint8_t[dataLength];

      int8_t busCheck = puppet->dataHasBeenRequested();
      if (busCheck == 1) { //data request
        puppet->reply(testData);
      }
      if (iter == ITERS) {
        delete puppet;
        dataLength *= 2;
        puppet = new LIN::Puppet(PUPPET_ID, BAUD_RATE, dataLength);
        delay(100); //give puppet time to clean
        iter = 0;
        trial++;
      }
    }
  }
}