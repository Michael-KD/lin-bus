#include <Arduino.h>
#include "lin.h"

using namespace LIN_Utils;

LIN_Master::LIN_Master(HardwareSerial* serialPort, uint32_t baudRate) {
    _serial = serialPort;
    this->baudRate = baudRate;
    _serial->begin(baudRate);
}

uint8_t* LIN_Master::requestData(uint8_t id) {
    uint8_t headerFrame[4] = {0};
    generateHeader(id, headerFrame);

    //clear receiving buffer
    while (_serial->available())
        _serial->read();

    //send header
    _serial->write(headerFrame, 4);

    //read data in
   
   return NULL;
}

void LIN_Master::generateHeader(uint8_t id, uint8_t* frame) {
    // break, 13 dominant bits (nominal) followed by a break delimiter of one bit (nominal) recessive
    frame[0] = 0x3f;
    frame[1] = 0xff;
    
    // sync, x55
    frame[2] = 0x55;

    // id, 1 byte -> 6 bit id, then 2 bit parity in MSBs
    frame[3] = (id & 0x3f) | parity(id);
}

//parity should be bits 6, 7, all other bits are 0
uint8_t parity(uint8_t id) {
    int bit6 = ((id >> 0) + (id >> 1) + (id >> 2) + (id >> 4)) & 1;
    int bit7 = ~((id >> 1) + (id >> 3) + (id >> 4) + (id >> 5)) & 1;
    return ((bit6 | (bit7 << 1)) << 6);
}

// =============================================================================================== //

LIN_Puppet::LIN_Puppet(HardwareSerial* serialPort, uint8_t id, uint32_t baudRate) {
    _serial = serialPort;
    this->id = id;
    this->baudRate = baudRate;
    _serial->begin(baudRate);
}

bool LIN_Puppet::getDataRequested() {
    //checks bus buffer for input, add to internal buffer
    //if buffer is complete, check PID and handle accordingly
    uint8_t id = 0; //TODO
    LIN_Puppet::compareID(id);
    
    return false;
}

void LIN_Puppet::reply(uint8_t* data) {
    uint8_t frame[DATA_SIZE + 1] = {0};
    LIN_Puppet::generateResponse(data, frame);
}

void LIN_Puppet::generateResponse(uint8_t* data, uint8_t* frame) {
    frame[0] = 0;
    //populate 
    //generate CRC
}

bool LIN_Puppet::compareID(uint8_t id) {
    //check if ID is the same as this puppet's ID, check parity
    if (LIN_Utils::parity(id) == (id & 0xc0)) {
        if (id == this->id) {
            return true;
        }
    }
    return false;
}
