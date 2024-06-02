#pragma once

#include "bus.hpp"

class Display : public BusDeviceWritable {
public:
    void busPortWrite(u8 data) { _lastSegmentValue = data; }

    void lightup(u8 digitValue) {
        int digit = 0;
        u8 i;

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

    u8 get(u8 digit) {
        _lightoffTimer--;

        if (_lightoffTimer <= 0) {
            _lightoffTimer = 0;
            return 0x00;
        }

        return _digits[digit]; 
    };

private:
    u8 _digits[6] = {0};
    u8 _lastSegmentValue = 0x00;
    int _lightoffTimer = 0;
};