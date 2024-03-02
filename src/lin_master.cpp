#include "lin.h"

using namespace LIN;

Master::Master(uint32_t baudRate, size_t dataSize) {
    this->baudRate = baudRate;
    this->dataSize = dataSize;
    _incDataBuffer = new uint8_t[dataSize + HEADER_SIZE + 2];
    enabled = false;
}

Master::~Master() {
    _serial->end();
    delete [] _incDataBuffer;
}

void Master::startSerial(HardwareSerial* serialPort) {
    _serial = serialPort;
    _serial->begin(baudRate);
}

bool Master::requestData(uint8_t* dataBuffer, uint8_t id) {
    if (!enabled)
        return false;
    uint8_t headerFrame[HEADER_SIZE] = {0};
    generateHeader(id, headerFrame);
    
    print("Header:");
    printArr(headerFrame, HEADER_SIZE);

    //send header
    _serial->write(headerFrame, HEADER_SIZE);

    //clear receiving buffer
    clearDataBuffer();
    while (_serial->available())
        _serial->read();
    clearDataBuffer();

    //read data in
    size_t j = 0;
    while (j < dataSize + HEADER_SIZE + 1) {
        if (_serial->available()) {
            _incDataBuffer[j] = _serial->read();
            j++;
        }
    }
    
    print("Incoming data buffer:");
    printArr(_incDataBuffer, dataSize + HEADER_SIZE + 1);

    if (_incDataBuffer[dataSize + HEADER_SIZE] == CRC(_incDataBuffer, HEADER_SIZE, dataSize)) {
        for (size_t i = 0; i < dataSize; i++) {
            dataBuffer[i] = _incDataBuffer[i + HEADER_SIZE];
        }
        return true;
    }
    return false;
}

void Master::generateHeader(uint8_t id, uint8_t* frame) {
    // break, 13 dominant bits (nominal) followed by a break delimiter of one bit (nominal) recessive
    frame[0] = 0x3f;
    frame[1] = 0xfe;
    
    // sync, x55
    frame[2] = 0x55;

    // id, 1 byte -> 6 bit id, then 2 bit parity in MSBs
    frame[3] = (id & 0x3f) | parity(id);
}

void Master::clearDataBuffer() {
    for (size_t i = 0; i < dataSize + HEADER_SIZE + 2; i++) {
        _incDataBuffer[i] = 0;
    }
}

void Master::enable() {
    enabled = true;
}

void Master::disable() {
    enabled = false;
}