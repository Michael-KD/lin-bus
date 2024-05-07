#include "lin.h"

using namespace LIN;

Puppet::Puppet(uint8_t id, uint32_t baudRate, size_t dataSize) {
    this->id = id;
    this->baudRate = baudRate;
    this->dataSize = dataSize;
    _incDataBuffer = new uint8_t[dataSize + 1];
    headerDetectionBuffer = 0;
    enabled = false;
}


Puppet::~Puppet() {
    _serial->end();
    delete[] _incDataBuffer;
}

void Puppet::startSerial(HardwareSerial* serialPort) {
    _serial = serialPort;
    _serial->begin(baudRate);
}

int8_t Puppet::dataHasBeenRequested() {
    if (!enabled)
        return false;
    //checks bus buffer for input, add to internal buffer
    int32_t headerIndex = -1;
    while (_serial->available()) {
        uint8_t incByte = _serial->read();
        headerDetectionBuffer = (headerDetectionBuffer << 8) | incByte;
        headerIndex = scan(0x3ffe55, headerDetectionBuffer, 0x7fffff, 23);
        if (headerIndex >= 8) {
            break;
        }
    }
    //if buffer is complete, check PID and handle accordingly
    if (headerIndex >= 8) {
        uint8_t pid = uint8_t((headerDetectionBuffer >> (headerIndex - 8)) & 0xff);
        headerDetectionBuffer = 0; //reset buffer
        if (Puppet::compareID(pid)) {
            return 1;
        } else if (pid == parity(0)) {
            return 2;
        }
    }
    return 0;
}

bool Puppet::readTransmittedData(uint8_t* dataBuffer) {
    //read data in
    size_t readBytes = 0;
    while (readBytes < dataSize + 1) {
        if (_serial->available()) {
            _incDataBuffer[readBytes] = _serial->read();
            readBytes++;
        }
    }
    if (_incDataBuffer[dataSize] == CRC(_incDataBuffer, 0, dataSize)) {
        for (size_t i = 0; i < dataSize; i++) {
            dataBuffer[i] = _incDataBuffer[i];
        }
        clearDataBuffer();
        return true;
    }
    clearDataBuffer();
    return false;
}

void Puppet::clearDataBuffer() {
    for (size_t i = 0; i < dataSize + 6; i++) {
        _incDataBuffer[i] = 0;
    }
}

void Puppet::reply(uint8_t* data) {
    if (!enabled)
        return;
    uint8_t frame[dataSize + 1] = {0};
    Puppet::generateResponse(data, frame);
    _serial->write(frame, dataSize + 1);
}

void Puppet::generateResponse(uint8_t* data, uint8_t* frame) {
    for (uint32_t i = 0; i < dataSize; i++) {
        frame[i] = data[i];
    }
    frame[dataSize] = CRC(data, 0, dataSize);
}

bool Puppet::compareID(uint8_t pid) {
    //check if ID is the same as this puppet's ID, check parity
    if (parity(pid) == (pid & 0xc0)) {
        if ((pid & 0x3f) == id) {
            return true;
        }
    }
    return false;
}

void Puppet::enable() {
    enabled = true;
}

void Puppet::disable() {
    enabled = false;
}