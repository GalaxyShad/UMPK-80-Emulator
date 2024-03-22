#pragma once

#include "bus.hpp"

#include <iostream>

class Display : public BusDeviceWritable {
public:
    void busPortWrite(uint8_t data) { _lastSegmentValue = data; }

    void lightup(uint8_t digitValue) {
        int digit = 0;
        uint8_t i;

        // Blinking fix
        if (digitValue != 0b100000 && digitValue != 0b010000 &&
            digitValue != 0b001000 && digitValue != 0b000100 &&
            digitValue != 0b000010 && digitValue != 0b000001)
            return;

        for (digit = 0, i = 0b100000;
             ((digitValue & i) == 0) && (digit < 6) && (i != 0);
             digit++, i = i >> 1)
            ;

        if (digit == 6)
            return;

        _digits[digit] = _lastSegmentValue;

        _lightoffTimer = 255;
    }

    uint8_t get(uint8_t digit) { 
        _lightoffTimer--;

        if (_lightoffTimer <= 0) {
            _lightoffTimer = 0;
            return 0x00;
        }

        return _digits[digit]; 
    };

private:
    uint8_t _digits[6] = {0};
    uint8_t _lastSegmentValue = 0x00;
    int _lightoffTimer = 0;
};