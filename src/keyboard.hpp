#pragma once

#include "bus.hpp"


#ifdef EMULATE_OLD_UMPK
    #define KEYBOARD_PORT_SCAN     0x28
    #define KEYBOARD_PORT_OUT      0x18
#else
    #define KEYBOARD_PORT_SCAN     0x07
    #define KEYBOARD_PORT_OUT      0x06
#endif



class Keyboard {
    public:
        Keyboard(Bus& bus) : _bus(bus) {}

        enum Key {
                R,     SHC,         _C, _D, _E, _F,
                SHK,   PR_SCH,      _8, _9, _A, _B,
            ST, OT_RG, OT_A,        _4, _5, _6, _7,
            P,  UM,    ZP_UV,       _0, _1, _2, _3,
        };

        void setKey(Key key, bool state) {
            _keys[key] = state; 
        }

        void update() {
            uint8_t scanCode = _bus.getOutPortData(KEYBOARD_PORT_SCAN);

            uint8_t value = 0xFF;

            switch (scanCode) {
                case 0b01111111: value = _keys[_D] ? 0xFE : _keys[_E] ? 0xFD : _keys[_F] ? 0xFB : 0xFF; break;
                case 0b10111111: value = _keys[_A] ? 0xFE : _keys[_B] ? 0xFD : _keys[_C] ? 0xFB : 0xFF; break;
                case 0b11011111: value = _keys[_7] ? 0xFE : _keys[_8] ? 0xFD : _keys[_9] ? 0xFB : 0xFF; break;
                case 0b11101111: value = _keys[_4] ? 0xFE : _keys[_5] ? 0xFD : _keys[_6] ? 0xFB : 0xFF; break;
                case 0b11110111: value = _keys[_1] ? 0xFE : _keys[_2] ? 0xFD : _keys[_3] ? 0xFB : 0xFF; break;
                case 0b11111011: value = _keys[_0] ? 0xFE : _keys[ZP_UV] ? 0xFD : _keys[UM] ? 0xFB : 0xFF; break;
                case 0b11111101: value = _keys[P]  ? 0xFE : _keys[OT_RG] ? 0xFD : _keys[OT_A] ? 0xFB : 0xFF; break;
                case 0b11111110: value = _keys[SHK] ? 0xFE : _keys[PR_SCH] ? 0xFD : 0xFF; break;
            }

            _bus.portInWrite(KEYBOARD_PORT_OUT, value);
        }

    private:
        Bus& _bus;
        bool _keys[26] = {false};

};