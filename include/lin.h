#pragma once

#include <Arduino.h>

namespace LIN {
    #define HEADER_SIZE 4

    /// @brief prints an array of characters in hex, for debug
    /// @param arr array
    /// @param len length
    void printArr(uint8_t* arr, size_t len);
    
    /// @brief prints a string (arduino, for debug)
    /// @param str string
    void print(String str);
    
    /// @brief prints an unsigned integer
    /// @param i a uint64_t
    void print(uint64_t i);
    
    /// @brief calculate the 2 parity bits (in msb and msb - 1) from an id
    /// @param id id
    /// @return the parity bits with 6 bits cleared for the actual id
    uint8_t parity(uint8_t id);
    
    /// @brief generates a checksum for an array of data
    /// @param data data buffer
    /// @param dataStart the start index of the data
    /// @param dataSize number of bytes in data
    /// @return byte-long checksum for the data
    uint8_t CRC(uint8_t* data, size_t dataStart, size_t dataSize);

    /// @brief Scans a 64-bit data buffer for a bit pattern
    /// @param pattern pattern to scan for
    /// @param data data to scan
    /// @param patternMask masks out the pattern
    /// @param patternLength number of bits in the pattern
    /// @return the offset by which data had to be shifted right to match with pattern; -1 if not found
    int64_t scan(uint64_t pattern, uint64_t data, uint64_t patternMask, size_t patternLength);

    class Puppet {
        public:
            /// @brief Constructor for the "puppet" process
            /// @param id id of the puppet
            /// @param baudRate bus baud rate
            /// @param dataSize size of frame data in bytes
            Puppet(uint8_t id, uint32_t baudRate, size_t dataSize);
            
            /// @brief Deconstructor
            ~Puppet();
            
            /// @brief prepares the serial port to send and receive data
            /// @param serialPort address of serial port for bus communication
            void startSerial(HardwareSerial* serialPort);
            
            /// @brief Send a reply to bus
            /// @param data data to be sent to bus
            void reply(uint8_t* data); //respond to a header
            
            /// @brief checks the input buffer to see if a request has been made to this node
            /// @return 1 if a request has been made to this node, 2 if a message has been transmitted, 0 if the request is not for this node
            int8_t dataHasBeenRequested(); //reads bus; returns true if header is itself
            
            /// @brief reads the data transmitted from masqueradingMaster
            /// @param dataBuffer buffer to store the incoming data
            /// @return if the checksum is valid and data integrity is not compromised
            bool readTransmittedData(uint8_t* _dataBuffer);

            /// @brief enable node
            void enable();
            
            /// @brief disable node
            void disable();
        private:
            HardwareSerial* _serial;
            uint8_t id;
            uint32_t baudRate;
            uint64_t headerDetectionBuffer;
            size_t dataSize;
            bool enabled;
            elapsedMicros timeSinceHeaderReceived;
            /// @brief Generates the response frame 
            /// @param data data to be used in data frame
            /// @param frame space allocated for the outgoing response
            void generateResponse(uint8_t* data, uint8_t* frame);
            
            /// @brief checks the PID against the puppet's own PID
            /// @param pid PID to check against
            /// @return if the PIDs match (ID and parity bits)
            bool compareID(uint8_t pid);
    };

    class Master {
        public:
            /// @brief Construct a "master" process.
            /// @param baudRate the baud rate of the LIN port
            /// @param dataSize the number of bytes to be expected when receiving data
            Master(uint32_t baudRate, size_t dataSize);

            /// @brief Deconstructor
            ~Master();
            
            /// @brief prepares the serial port to send and receive data
            /// @param serialPort pointer to the serial port to be used for bus communication
            void startSerial(HardwareSerial* serialPort);
            
            /// @brief requests data from a node on the bus
            /// @param dataBuffer an array to hold the incoming data
            /// @param id the "puppet" id to query
            /// @return if the checksum is valid and data integrity is not compromised
            bool requestData(uint8_t* dataBuffer, uint8_t id);
            
            /// @brief transmits data to the bus
            /// @param data data to transmit to bus
            /// @return if idk bruh
            bool transmitData(uint8_t* data);

            /// @brief Enables the "master" process
            void enable();
            
            /// @brief Disables the "master" process
            void disable();
        private:
            HardwareSerial* _serial;
            uint32_t baudRate;
            uint8_t* _incDataBuffer; //will be dataSize + 2 (1 for CRC, other to handle 1-bit offset)
            size_t dataSize;
            bool enabled;
            /// funky guy
            LIN::Puppet* masqueradingMaster;
            /// @brief Generates the LIN header frame
            /// @param id id of puppet node to contact
            /// @param frame pointer to the space allocated for the frame
            void generateHeader(uint8_t id, uint8_t* frame);
            /// @brief clears the incoming data buffer
            void clearDataBuffer();
    };
}