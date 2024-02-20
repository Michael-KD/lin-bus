#include "lin.h"

using namespace LIN;

//parity should be bits 6, 7, all other bits are 0
uint8_t parity(uint8_t id) {
    int bit6 = ((id >> 0) + (id >> 1) + (id >> 2) + (id >> 4)) & 1;
    int bit7 = ~((id >> 1) + (id >> 3) + (id >> 4) + (id >> 5)) & 1;
    return ((bit6 | (bit7 << 1)) << 6);
}

uint8_t CRC(uint8_t* data, size_t dataSize) {
    uint8_t sum = 0;
    for (size_t i = 0; i < dataSize; i++) {
        sum += data[i];
    }
    return sum;
}

uint64_t scan(uint8_t* pattern, uint8_t* data, size_t patternLength, size_t dataLength) {
    return false;
}

//patternMask should be a bitmask where all bits in the pattern are 1
//returns the number of bits the data needs to be shifted by to detect the pattern
int32_t scan(uint64_t pattern, uint64_t data, uint64_t patternMask, size_t patternLength) {
    for (size_t i = 0; i <= (64 - patternLength); i++) {
        if (pattern == ((data >> i) & patternMask)) {
            return i;
        }
    }
    return -1;
}
    

Master::Master(HardwareSerial* serialPort, uint32_t baudRate, size_t dataSize) {
    _serial = serialPort;
    this->baudRate = baudRate;
    this->dataSize = dataSize;
    _serial->begin(baudRate);
    uint8_t incDataBuffer[dataSize + 2] = {0};
    _incDataBuffer = incDataBuffer;
}

void Master::requestData(uint8_t* dataBuffer, uint8_t id) {
    uint8_t headerFrame[4] = {0};
    generateHeader(id, headerFrame);

    //clear receiving buffer
    clearDataBuffer();
    while (_serial->available())
        _serial->read();

    //send header
    _serial->write(headerFrame, 4);

    //read data in
    size_t j = 0;
    while (j < dataSize + 2) {
        if (_serial->available()) {
            _incDataBuffer[j] = _serial->read();
            j++;
        }
    }

    //buffer will be 1 bit shifted to the right of the actual data
    //so this is to shift everything left
    for (size_t i = 0; i < dataSize + 1; i++) {
        _incDataBuffer[i] = (_incDataBuffer[i] << 1) | (_incDataBuffer[i + 1] >> 7);
    }
    
    if (_incDataBuffer[dataSize] == CRC(_incDataBuffer, dataSize)) {
        for (size_t i = 0; i < dataSize; i++) {
            dataBuffer[i] = _incDataBuffer[i];
        }
    }
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
    for (size_t i = 0; i < dataSize * 2; i++) {
        _incDataBuffer[i] = 0;
    }
}

// =============================================================================================== //

Puppet::Puppet(HardwareSerial* serialPort, uint8_t id, uint32_t baudRate, size_t dataSize) {
    _serial = serialPort;
    this->id = id;
    this->baudRate = baudRate;
    this->dataSize = dataSize;
    _serial->begin(baudRate);
    headerDetectionBuffer = 0;
}

bool Puppet::dataHasBeenRequested() {
    //checks bus buffer for input, add to internal buffer
    int32_t headerIndex = -1;
    while (_serial->available()) {
        uint8_t incByte = _serial->read();
        headerDetectionBuffer = (headerDetectionBuffer << 8) | incByte;
        headerIndex = scan(0x3ffe55, headerDetectionBuffer, 0x8fffff, 23);
        if (headerIndex >= 8) {
            break;
        }
    }

    //if buffer is complete, check PID and handle accordingly
    if (headerIndex >= 8) {
        uint8_t pid = uint8_t((headerDetectionBuffer >> (headerIndex - 8)) & 0xff);
        headerDetectionBuffer = 0; //reset buffer
        if (Puppet::compareID(pid)) {
            timeSinceHeaderReceived = 0;
            return true;
        }
    }

    return false;
}

void Puppet::reply(uint8_t* data) {
    uint8_t frame[dataSize + 1] = {0};
    Puppet::generateResponse(data, frame);
    //wait for a bit (literally)
    if (timeSinceHeaderReceived < 1000000 / baudRate)
        delay(1000000 / baudRate - timeSinceHeaderReceived);
    _serial->write(frame, dataSize + 1);
}

void Puppet::generateResponse(uint8_t* data, uint8_t* frame) {
    for (uint32_t i = 0; i < dataSize; i++) {
        frame[i] = data[i];
    }
    frame[dataSize] = CRC(data, dataSize);
}

bool Puppet::compareID(uint8_t pid) {
    //check if ID is the same as this puppet's ID, check parity
    if (parity(pid) == (pid & 0xc0)) {
        if (pid == id) {
            return true;
        }
    }
    return false;
}
