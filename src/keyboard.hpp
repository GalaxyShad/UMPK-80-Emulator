#pragma once

#include "register.hpp"

#define KEYBOARD_KEYS_COUNT      24
#define KEYBOARD_COLUMNS_COUNT   3      
#define KEYBOARD_ROWS_COUNT      8             

class Keyboard : public BusDeviceReadable {
    public:
        Keyboard(RegisterScanDevice& scan) : _scan(scan) {}

        // enum Key {
        //         R,     SHC,         _C, _D, _E, _F,
        //         SHK,   PR_SCH,      _8, _9, _A, _B,
        //     ST, OT_RG, OT_A,        _4, _5, _6, _7,
        //     P,  UM,    ZP_UV,       _0, _1, _2, _3,
        // };

        uint8_t busPortRead() { return scan(_scan.busPortRead()); }

        enum Key {
            _D,     _E,     _F,
            _A,     _B,     _C,
            _7,     _8,     _9, 
            _4,     _5,     _6, 
            _1,     _2,     _3,
            _0,     ZP_UV,  UM,
            P,      OT_RG,  OT_A,
            SHK,    PR_SCH, SHC,
            R, ST
        };

        void keySetState(Key key, bool state) {
            _keys[key] = state; 
        }

        bool isKeyPressed(Key key)   { return _keys[key]; }
        bool isKeyReleased(Key key)  { return !_keys[key]; }

        void keyPress(Key key)      { keySetState(key, true); }
        void keyRelease(Key key)    { keySetState(key, false); }

        uint8_t scan(uint8_t scanValue) {

            uint8_t s, i;
            for (
                s = 0x80, i = 0; 
                ((s & ~scanValue) == 0) && (i < KEYBOARD_ROWS_COUNT); 
                s = (s >> 1), i++
            );

            // printf("LOL %02x %02x\n", s, scanValue);

            return _scanRow(i);

            // switch (scanValue) {
            //     case 0b01111111: value = _keys[_D] ? 0xFE : _keys[_E] ? 0xFD : _keys[_F] ? 0xFB : 0xFF; break;
            //     case 0b10111111: value = _keys[_A] ? 0xFE : _keys[_B] ? 0xFD : _keys[_C] ? 0xFB : 0xFF; break;
            //     case 0b11011111: value = _keys[_7] ? 0xFE : _keys[_8] ? 0xFD : _keys[_9] ? 0xFB : 0xFF; break;
            //     case 0b11101111: value = _keys[_4] ? 0xFE : _keys[_5] ? 0xFD : _keys[_6] ? 0xFB : 0xFF; break;
            //     case 0b11110111: value = _keys[_1] ? 0xFE : _keys[_2] ? 0xFD : _keys[_3] ? 0xFB : 0xFF; break;
            //     case 0b11111011: value = _keys[_0] ? 0xFE : _keys[ZP_UV] ? 0xFD : _keys[UM] ? 0xFB : 0xFF; break;
            //     case 0b11111101: value = _keys[P]  ? 0xFE : _keys[OT_RG] ? 0xFD : _keys[OT_A] ? 0xFB : 0xFF; break;
            //     case 0b11111110: value = _keys[SHK] ? 0xFE : _keys[PR_SCH] ? 0xFD : 0xFF; break;
            // }
        }


    private:
        uint8_t _scanRow(int row) {
            int index = row * KEYBOARD_COLUMNS_COUNT;

            for (int i = index; (i < index+KEYBOARD_COLUMNS_COUNT) && (i < KEYBOARD_KEYS_COUNT); i++) {
                uint8_t value = 0b1 << (i % KEYBOARD_COLUMNS_COUNT);

                if (_keys[i]) return ~value;
            }

            return 0xFF;
        }

        bool _keys[KEYBOARD_KEYS_COUNT] = {false};

        RegisterScanDevice& _scan;

};