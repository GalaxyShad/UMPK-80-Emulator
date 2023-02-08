#pragma once

#include <iostream>

#include "bus.hpp"

#ifdef EMULATE_OLD_UMPK
    #define DISPLAY_PORT_SEGMENT     0x28
    #define DISPLAY_PORT_DATA        0x38
#else
    #define DISPLAY_PORT_SEGMENT     0x07
    #define DISPLAY_PORT_DATA        0x06
#endif
class Display {
    public:
        Display(Bus& bus) : _bus(bus) {};

        void update() {
            uint8_t seg   = _bus.getOutPortData(DISPLAY_PORT_SEGMENT);
            uint8_t data  = _bus.getOutPortData(DISPLAY_PORT_DATA);

            switch (seg) {
                case 0b00000001: _segments[5] = data; break;
                case 0b00000010: _segments[4] = data; break;
                case 0b00000100: _segments[3] = data; break;
                case 0b00001000: _segments[2] = data; break;

                case 0b00010000: _segments[1] = data; break;
                case 0b00100000: _segments[0] = data; break;   
                default: return; 
            }

            _bus.portWrite(DISPLAY_PORT_SEGMENT, 0x00);
        }

        uint8_t get(uint8_t segment) {
            return _segments[segment];
        };
    private:
        uint8_t _segments[6] = {0};
        Bus&    _bus;
};