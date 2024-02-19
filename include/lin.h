#pragma once

#include <Arduino.h>

namespace LIN {
    uint8_t parity(uint8_t id);
    uint8_t CRC(uint8_t* data);
    
    //scan data for a given pattern
    uint64_t scan(uint8_t* pattern, uint8_t* data, size_t patternLength, size_t dataLength);
    int32_t scan(uint64_t pattern, uint64_t data, uint64_t patternMask, size_t patternLength);

    class Master {
        public:
            Master(HardwareSerial* serialPort, uint32_t baudRate, uint32_t dataSize);
            uint8_t* requestData(uint8_t id); //send a header
        private:
            HardwareSerial* _serial;
            uint32_t baudRate;
            uint8_t* _incDataBuffer; //will be dataSize * 2
            uint32_t dataSize;
            void generateHeader(uint8_t id, uint8_t* frame);
    };

    class Puppet {
        public:
            Puppet(HardwareSerial* serialPort, uint8_t id, uint32_t baudRate, uint32_t dataSize);
            void reply(uint8_t* data); //respond to a header
            bool dataHasBeenRequested(); //reads bus; returns true if header is itself
        private:
            HardwareSerial* _serial;
            uint8_t id;
            uint32_t baudRate;
            uint64_t headerDetectionBuffer;
            uint32_t dataSize;
            void generateResponse(uint8_t* data, uint8_t* frame);
            bool compareID(uint8_t id);
    };
}