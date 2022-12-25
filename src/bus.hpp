#pragma once

#include <cinttypes>

class Bus {
    public:
        void    write(uint16_t adr, uint8_t data) {
            _memory[adr] = data;
        }

        uint8_t read(uint16_t adr) {
            return _memory[adr];
        }

        uint8_t _memory[0xFFFF];

    private:
};