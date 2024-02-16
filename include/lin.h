#pragma once

#include <Arduino.h>

#define DATA_SIZE 8

namespace LIN {
    uint8_t parity(uint8_t id);

    class Master {
        public:
            Master(HardwareSerial* serialPort, uint32_t baudRate);
            uint8_t* requestData(uint8_t id); //send a header
        private:
            HardwareSerial* _serial;
            uint32_t baudRate;
            void generateHeader(uint8_t id, uint8_t* frame);
    };

    class Puppet {
        public:
            Puppet(HardwareSerial* serialPort, uint8_t id, uint32_t baudRate);
            void reply(uint8_t* data); //respond to a header
            bool getDataRequested(); //gets header from master; returns true if header is itself
        private:
            HardwareSerial* _serial;
            uint8_t id;
            uint32_t baudRate;
            void generateResponse(uint8_t* data, uint8_t* frame);
            bool compareID(uint8_t id);
    };
}