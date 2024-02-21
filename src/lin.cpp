#include "lin.h"

using namespace LIN;

const bool DEBUG = true;

//for debug
void LIN::printArr(uint8_t* arr, size_t len) {
    if (!DEBUG) return;
    for (size_t i = 0; i < len; i++) {
        Serial.print(arr[i], HEX);
        Serial.print("_");
    }
    Serial.println();
}
void LIN::print(String str) {
    if (!DEBUG) return;
    Serial.println(str);
}
void LIN::print(uint64_t i) {
    if (!DEBUG) return;
    Serial.println(i, HEX);
}

//parity should be bits 6, 7, all other bits are 0
uint8_t LIN::parity(uint8_t id) {
    int bit6 = ((id >> 0) + (id >> 1) + (id >> 2) + (id >> 4)) & 1;
    int bit7 = ~((id >> 1) + (id >> 3) + (id >> 4) + (id >> 5)) & 1;
    return ((bit6 | (bit7 << 1)) << 6);
}

uint8_t LIN::CRC(uint8_t* data, size_t dataSize) {
    uint8_t sum = 0;
    for (size_t i = 0; i < dataSize; i++) {
        sum += data[i];
    }
    return sum;
}

//patternMask should be a bitmask where all bits in the pattern are 1
//returns the number of bits the data needs to be shifted by to detect the pattern
size_t LIN::scan(uint64_t pattern, uint64_t data, uint64_t patternMask, size_t patternLength) {
    for (size_t i = 0; i <= (64 - patternLength); i++) {
        if (pattern == ((data >> i) & patternMask)) {
            return i;
        }
    }
    return -1;
}
    
/* =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== */

Master::Master(uint32_t baudRate, size_t dataSize) {
    this->baudRate = baudRate;
    this->dataSize = dataSize;
    _incDataBuffer = new uint8_t[dataSize + 2];
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
    uint8_t headerFrame[4] = {0};
    generateHeader(id, headerFrame);
    
    print("Header:");
    printArr(headerFrame, 4);

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
    
    print("Incoming data buffer:");
    printArr(_incDataBuffer, dataSize);

    if (_incDataBuffer[dataSize] == CRC(_incDataBuffer, dataSize)) {
        for (size_t i = 0; i < dataSize; i++) {
            dataBuffer[i] = _incDataBuffer[i];
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
    for (size_t i = 0; i < dataSize * 2; i++) {
        _incDataBuffer[i] = 0;
    }
}

void Master::enable() {
    enabled = true;
}

void Master::disable() {
    enabled = false;
}

/* =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
//                     _.---._    /\\                                                              //
//                  ./'       "--`\//
//                ./              o \          .-----.
//               /./\  )______   \__ \        ( help! )
//              ./  / /\ \   | \ \  \ \       /`-----'
//                 / /  \ \  | |\ \  \7--- ooo ooo ooo ooo ooo ooo
// =============================================================================================== //
// =============================================================================================== //
//                  _      _      _
//                >(.)__ <(.)__ =(.)__
//                 (___/  (___/  (___/
//
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== //
// =============================================================================================== */

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

bool Puppet::dataHasBeenRequested() {
    if (!enabled)
        return false;
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

    print("Header detection buffer:");
    print(headerDetectionBuffer);

    //if buffer is complete, check PID and handle accordingly
    if (headerIndex >= 8) {
        print("Buffer detected.");
        uint8_t pid = uint8_t((headerDetectionBuffer >> (headerIndex - 8)) & 0xff);
        
        headerDetectionBuffer = 0; //reset buffer
        if (Puppet::compareID(pid)) {
            print("PID match.");
            timeSinceHeaderReceived = 0;
            return true;
        }
        print("No PID match.");
    }

    return false;
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

void Puppet::enable() {
    enabled = true;
}

void Puppet::disable() {
    enabled = false;
}