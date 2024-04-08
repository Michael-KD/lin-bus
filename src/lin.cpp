#include "lin.h"

using namespace LIN;

//parity should be bits 6, 7, all other bits are 0
uint8_t LIN::parity(uint8_t id) {
    int bit6 = ((id >> 0) + (id >> 1) + (id >> 2) + (id >> 4)) & 1;
    int bit7 = ~((id >> 1) + (id >> 3) + (id >> 4) + (id >> 5)) & 1;
    return ((bit6 | (bit7 << 1)) << 6);
}

uint8_t LIN::CRC(uint8_t* data, size_t dataStart, size_t dataSize) {
    uint8_t sum = 0;
    for (size_t i = 0; i < dataSize; i++) {
        sum += data[i + dataStart];
    }
    return sum;
}

//patternMask should be a bitmask where all bits in the pattern are 1
//returns the number of bits the data needs to be shifted by to detect the pattern
int64_t LIN::scan(uint64_t pattern, uint64_t data, uint64_t patternMask, size_t patternLength) {
    for (size_t i = 0; i <= (64 - patternLength); i++) {
        if (pattern == ((data >> i) & patternMask)) {
            return i;
        }
    }
    return -1;
}