#include <Arduino.h>

class LIN_Master {
    public:
        LIN_Master(HardwareSerial* serialPort, uint32_t baudRate);
        uint8_t* requestData(uint8_t id); //send a header
    private:
        HardwareSerial* _serial;
        uint32_t baudRate;
        uint8_t* generateHeader(uint8_t id);
};

class LIN_Puppet {
    public:
        LIN_Puppet();
        void reply(uint8_t* data, size_t dataLength); //respond to a header
        int get(); //gets header from master; returns the PID
    private:
        uint8_t* generateResponse();
};