#pragma once

#include <Arduino.h>

namespace LIN {
    void printArr(uint8_t* arr, size_t len);
    void print(String str);
    void print(uint64_t i);
    uint8_t parity(uint8_t id);
    uint8_t CRC(uint8_t* data, size_t dataSize);
    
    //scan data for a given pattern
    int64_t scan(uint64_t pattern, uint64_t data, uint64_t patternMask, size_t patternLength);

    class Master {
        public:
            Master(uint32_t baudRate, size_t dataSize);
            ~Master();
            void startSerial(HardwareSerial* serialPort);
            bool requestData(uint8_t* dataBuffer, uint8_t id); //send a header
            void enable();
            void disable();
        private:
            HardwareSerial* _serial;
            uint32_t baudRate;
            uint8_t* _incDataBuffer; //will be dataSize + 2 (1 for CRC, other to handle 1-bit offset)
            size_t dataSize;
            bool enabled;
            void generateHeader(uint8_t id, uint8_t* frame);
            void clearDataBuffer();
    };

    class Puppet {
        public:
            Puppet(uint8_t id, uint32_t baudRate, size_t dataSize);
            ~Puppet();
            void startSerial(HardwareSerial* serialPort);
            void reply(uint8_t* data); //respond to a header
            bool dataHasBeenRequested(); //reads bus; returns true if header is itself
            void enable();
            void disable();
        private:
            HardwareSerial* _serial;
            uint8_t id;
            uint32_t baudRate;
            uint64_t headerDetectionBuffer;
            size_t dataSize;
            bool enabled;
            elapsedMicros timeSinceHeaderReceived;
            void generateResponse(uint8_t* data, uint8_t* frame);
            bool compareID(uint8_t pid);
    };
}