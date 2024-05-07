// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <Arduino.h>
// #include "lin.h"

// const uint32_t BAUD_RATE = 19200;

// //pins
// const uint8_t LIN_RXD = 0;
// const uint8_t LIN_TXD = 1;
// const uint8_t LIN_MCP_RESET = 2;
// const uint8_t LIN_CS = 3;

// const uint8_t UP = 7;
// const uint8_t DOWN = 6;

// //ids 
// const uint8_t PUPPET_ID = 0x3b; //completely arbitrary

// size_t dataLength = 1;

// LIN::Puppet* puppet;

// uint8_t buttonState;

// void setup() {
//     Serial.begin(9600);
//     pinMode(LIN_CS, OUTPUT);
//     digitalWrite(LIN_CS, HIGH);
//     pinMode(UP, INPUT);
//     pinMode(DOWN, INPUT);
//     buttonState = 0;
//     puppet = new LIN::Puppet(PUPPET_ID, BAUD_RATE, dataLength);
  
//     puppet->startSerial(&Serial1);
//     puppet->enable();
// }

// void loop() {
//     int8_t busCheck = puppet->dataHasBeenRequested();
//     int8_t up = !digitalRead(UP);
//     int8_t down = !digitalRead(DOWN);
//     buttonState = up - down;
//     if (busCheck == 1) { //data request
//         puppet->reply(&buttonState);
//     } else if (busCheck == 2) { //data transmission
        
//     }
// }