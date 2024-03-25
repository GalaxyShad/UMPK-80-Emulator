#pragma once

#include "register.hpp"

#define KEYBOARD_KEYS_COUNT 24
#define KEYBOARD_COLUMNS_COUNT 3
#define KEYBOARD_ROWS_COUNT 8

// clang-format off
enum class KeyboardKey {
    _D,    _E,     _F,
    _A,    _B,     _C,
    _7,    _8,     _9, 
    _4,    _5,     _6, 
    _1,    _2,     _3,
    _0,    ZP_UV,  UM,
    P,     OT_RG,  OT_A,
    SHK,   PR_SCH, SHC,
    R,     ST
};
// clang-format on

class Keyboard : public BusDeviceReadable {
public:
    Keyboard(RegisterScanDevice &scan) : _scan(scan) {}

    uint8_t busPortRead() override { return scan(_scan.busPortRead()); }

    void setKeyState(KeyboardKey key, bool state) { _keys[(int)key] = state; }

    bool isKeyPressed(KeyboardKey key) { return _keys[(int)key]; }
    bool isKeyReleased(KeyboardKey key) { return !_keys[(int)key]; }

    void keyPress(KeyboardKey key) { setKeyState(key, true); }
    void keyRelease(KeyboardKey key) { setKeyState(key, false); }

    uint8_t scan(uint8_t scanValue) {

        uint8_t s, i;
        for (s = 0x80, i = 0;
             ((s & ~scanValue) == 0) && (i < KEYBOARD_ROWS_COUNT);
             s = (s >> 1), i++)
            ;

        return _scanRow(i);
    }

private:
    uint8_t _scanRow(int row) {
        int index = row * KEYBOARD_COLUMNS_COUNT;

        for (int i = index;
             (i < index + KEYBOARD_COLUMNS_COUNT) && (i < KEYBOARD_KEYS_COUNT);
             i++) {
            uint8_t value = 0b1 << (i % KEYBOARD_COLUMNS_COUNT);

            if (_keys[i])
                return ~value;
        }

        return 0xFF;
    }

    bool _keys[KEYBOARD_KEYS_COUNT] = {false};

    RegisterScanDevice &_scan;
};