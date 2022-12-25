#include <iostream>
#include <string>

#include "cpu.hpp"
#include "bus.hpp"

int main() {

    Bus bus;

    bus._memory[0x08] = 0x76;
    bus._memory[0x09] = 0xC9;

    uint8_t program[] = {
        0x21,
        0x02,
        0x09,
        0x4E,
        0x2B,
        0x7E,
        0x2B,
        0x86,
        0xDA,
        0x12,
        0x08,
        0x91,
        0xDA,
        0x23,
        0x08,
        0xC3,
        0x16,
        0x08,
        0x91,
        0xD2,
        0x1E,
        0x08,
        0xFE,
        0xF7,
        0xDA,
        0x23,
        0x08,
        0xC3,
        0x28,
        0x08,
        0xE6,
        0xFE,
        0xCA,
        0x28,
        0x08,
        0x3E,
        0x70,
        0xC3,
        0x2A,
        0x08,
        0x3E,
        0x07,
        0xD3,
        0x05,
        0xCF,
   };

    memcpy(
        bus._memory + 0x0800, 
        program, 
        45
    );


    bus._memory[0x0900] = 0xFF;
    bus._memory[0x0901] = 0x03;
    bus._memory[0x0902] = 0x00;

    for (int i = 0; i < 45; i++) {
        printf("%x %x\n", 0x0800 + i, bus._memory[0x0800 + i]);
    }

    Cpu cpu(bus);
    cpu.setProgramCounter(0x0800);
    while(!cpu.isHold()) {
        cpu.tick();

        printf("regCmd=0x%02x prgCounter=0x%04x\n", cpu._regCmd, cpu._prgCounter);
        printf(
            "A = %02x; B = %02x; C = %02x; D = %02x; E = %02x; H = %02x; L = %02x;\n",
            cpu._regA, cpu._regB, cpu._regC, cpu._regD, cpu._regE, cpu._regH, cpu._regL
        );
        printf(
            "C = %x; Z = %x; P = %x; S = %x;\n\n",
            cpu._regFlag.carry, cpu._regFlag.zero, cpu._regFlag.parry, cpu._regFlag.sign
        );
    }
    

    return 0;
}