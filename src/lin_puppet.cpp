#include "lin.h"

using namespace LIN;

Puppet::Puppet(uint8_t id, uint32_t baudRate, size_t dataSize) {
    this->id = id;
    this->baudRate = baudRate;
    this->dataSize = dataSize;
    headerDetectionBuffer = 0;
    enabled = false;
}


Puppet::~Puppet() {
    _serial->end();
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
        // print(incByte);
        headerDetectionBuffer = (headerDetectionBuffer << 8) | incByte;
        print(headerDetectionBuffer);
        // print("=====");
        headerIndex = scan(0x3ffe55, headerDetectionBuffer, 0x7fffff, 23);
        // print(headerIndex);
        if (headerIndex >= 8) {
            break;
        }
    }

    // print("Header detection buffer:");
    // print(headerDetectionBuffer);

    //if buffer is complete, check PID and handle accordingly
    if (headerIndex >= 8) {
        print("Buffer detected.");
        uint8_t pid = uint8_t((headerDetectionBuffer >> (headerIndex - 8)) & 0xff);
        print(pid);
        headerDetectionBuffer = 0; //reset buffer
        if (Puppet::compareID(pid)) {
            print("PID match.");
            timeSinceHeaderReceived = 0;
            return 1;
        } else if (pid == 0x80) {
            print("Broadcast detected.");
            return 2;
        }
        print("No PID match.");
    }

    return 0;
}

bool Puppet::readTransmittedData(uint8_t* _dataBuffer) {
    size_t readBytes = 0;

    while (readBytes < dataSize) {
        if (_serial->available()) {
            _dataBuffer[readBytes] = _serial->read();
        }
    }

    uint8_t crc = _serial->read();
    return (crc == CRC(_dataBuffer, 0, dataSize));
}

void Puppet::reply(uint8_t* data) {
    if (!enabled)
        return;
    uint8_t frame[dataSize + 1] = {0};
    Puppet::generateResponse(data, frame);
    //wait for a bit (literally)
    if (timeSinceHeaderReceived < 1000000 / baudRate)
        delay(1000000 / baudRate - timeSinceHeaderReceived);
    _serial->write(frame, dataSize + 1);
    printArr(frame, dataSize + 1);
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