#include <Arduino.h>
#include "lin.h"

LIN_Master::LIN_Master(HardwareSerial* serialPort, uint32_t baudRate) {
    _serial = serialPort;
    this->baudRate = baudRate;
    _serial->begin(baudRate);
}

uint8_t* LIN_Master::requestData(uint8_t id) {
    uint8_t* header = generateHeader(id);

    //clear the buffer
    while (_serial->available()) {
        _serial->read();
    }
}