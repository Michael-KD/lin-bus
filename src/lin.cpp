#include "lin.h"

using namespace LIN;

const bool DEBUG = true;

//for debug
void LIN::printArr(uint8_t* arr, size_t len) {
    if (!DEBUG) return;
    for (size_t i = 0; i < len - 1; i++) {
        Serial.print(arr[i], HEX);
        Serial.print(" ");
    }
    Serial.print(arr[len - 1], HEX);
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