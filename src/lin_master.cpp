#include "lin.h"

using namespace LIN;

Master::Master(uint32_t baudRate, size_t dataSize) {
    this->baudRate = baudRate;
    this->dataSize = dataSize;
    _incDataBuffer = new uint8_t[dataSize + HEADER_SIZE + 1];
    enabled = false;
    masqueradingMaster = new Puppet(0, 19200, dataSize);
    masqueradingMaster->enable();
}

Master::~Master() {
    _serial->end();
    delete masqueradingMaster;
    delete[] _incDataBuffer;
}

void Master::startSerial(HardwareSerial* serialPort) {
    _serial = serialPort;
    masqueradingMaster->startSerial(serialPort);
}

bool Master::requestData(uint8_t* dataBuffer, uint8_t id) {
    if (!enabled || id == 0)
        return false;
    uint8_t headerFrame[4] = {0};
    generateHeader(id, headerFrame);
    
    print("Header:");
    printArr(headerFrame, 4);

    //send header
    _serial->write(headerFrame, 4);

    //clear receiving buffer
    while (_serial->available())
        _serial->read();
    clearDataBuffer();

    //read data in
    size_t j = 0;
    while (j < dataSize + 5) {
        if (_serial->available()) {
            _incDataBuffer[j] = _serial->read();
            j++;
        }
    }

    // //buffer will be 1 bit shifted to the right of the actual data
    // //so this is to shift everything left
    // for (size_t i = 0; i < dataSize + 5; i++) {
    //     _incDataBuffer[i] = (_incDataBuffer[i] << 1) | (_incDataBuffer[i + 1] >> 7);
    // }
    
    print("Incoming data buffer:");
    printArr(_incDataBuffer, dataSize + 5);

    if (_incDataBuffer[dataSize + 4] == CRC(_incDataBuffer, 4, dataSize)) {
        for (size_t i = 0; i < dataSize; i++) {
            dataBuffer[i] = _incDataBuffer[i + 4];
        }
        return true;
    }
    return false;
}

bool Master::transmitData(uint8_t* data) {
    if (!enabled)
        return false;
    uint8_t headerFrame[HEADER_SIZE] = {0};
    generateHeader(0, headerFrame);
    
    print("Header:");
    printArr(headerFrame, HEADER_SIZE);

    //send header
    _serial->write(headerFrame, HEADER_SIZE);

    masqueradingMaster->reply(data);
    return true;
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
    for (size_t i = 0; i < dataSize + 6; i++) {
        _incDataBuffer[i] = 0;
    }
}

void Master::enable() {
    enabled = true;
}

void Master::disable() {
    enabled = false;
}