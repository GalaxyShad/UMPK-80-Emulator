#pragma once

#include <cinttypes>

class Bus {
    public:
        void write(uint16_t adr, uint8_t data) {
            _memory[adr] = data;
        }

        uint8_t read(uint16_t adr) {
            return _memory[adr];
        }

        void portWrite(uint8_t port, uint8_t data) {
            _outPorts[port] = data;
        }

        uint8_t getOutPortData(uint8_t port) {
            return _outPorts[port];
        }

        uint8_t portRead(uint8_t port) {
            return _inPorts[port];
        }

        void portInWrite(uint8_t port, uint8_t data) {
            _inPorts[port] = data;
        }



        

    private:
        uint8_t screenPointer = 0;
        uint8_t _memory[0xFFFF]  = {0};
        uint8_t _outPorts[256] = {0};
        uint8_t _inPorts[256]  = {0};

    private:
};