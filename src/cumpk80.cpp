#include <cstdint>

typedef void* UMPK80_t;

#include "core/umpk80.hpp"

extern "C" {
    enum UMPK80_Register {
        PC_LOW, PC_HIGH,
        SP_LOW, SP_HIGH,
        L,   H,
        E,   D,
        C,   B,
        PSW, A,
        M
    };

    enum UMPK80_RegisterPair {
        PC,
        SP,
        HL,
        DE,
        BC,
        PSWA,
    };

    UMPK80_t UMPK80_Create();
    void     UMPK80_Free(UMPK80_t umpk);

    void    UMPK80_PortIOSetInput(UMPK80_t umpk, uint8_t data);
    uint8_t UMPK80_PortIOGetInput(UMPK80_t umpk);

    uint8_t UMPK80_PortIOGetOutput(UMPK80_t umpk);

    void    UMPK80_Tick(UMPK80_t umpk);
    void    UMPK80_Stop(UMPK80_t umpk);
    void    UMPK80_Restart(UMPK80_t umpk);

    void    UMPK80_KeyboardPressButton(UMPK80_t umpk, uint8_t key);
    void    UMPK80_KeyboardReleaseButton(UMPK80_t umpk, uint8_t key);

    uint8_t UMPK80_DisplayGetDigit(UMPK80_t umpk, int digit);
    void    UMPK80_LoadOS(UMPK80_t umpk, const uint8_t* os);

    void    UMPK80_LoadProgram(UMPK80_t umpk, const uint8_t* program, uint16_t programSize, uint16_t dstAddress);

    uint16_t UMPK80_CpuProgramCounter(UMPK80_t umpk);
    void     UMPK80_CpuSetProgramCounter(UMPK80_t umpk, uint16_t value);
    uint16_t UMPK80_CpuStackPointer(UMPK80_t umpk);
    uint8_t  UMPK80_CpuGetRegister(UMPK80_t umpk, UMPK80_Register reg);

    uint8_t UMPK80_GetRegister(UMPK80_t umpk, UMPK80_Register reg);
    void    UMPK80_SetRegister(UMPK80_t umpk, UMPK80_Register reg, uint8_t value);

    uint16_t UMPK80_GetRegisterPair(UMPK80_t umpk, UMPK80_RegisterPair regPair);
    void     UMPK80_SetRegisterPair(UMPK80_t umpk, UMPK80_RegisterPair regPair, uint16_t value);

    uint8_t UMPK80_MemoryRead(UMPK80_t umpk, uint16_t adr);
    void    UMPK80_MemoryWrite(UMPK80_t umpk, uint16_t adr, uint8_t data);
}


UMPK80_t UMPK80_Create() {
    return new Umpk80();
}

void UMPK80_Free(UMPK80_t umpk) {
    delete (Umpk80*)umpk;
}

static Umpk80* inst(UMPK80_t umpk) { return (Umpk80*)umpk; }

void UMPK80_PortIOSetInput(UMPK80_t umpk, uint8_t data) {
    inst(umpk)->port5InSet(data); 
}

uint8_t UMPK80_PortIOGetInput(UMPK80_t umpk, uint8_t data) {
    return inst(umpk)->port5InGet(); 
}

uint8_t UMPK80_PortIOGetOutput(UMPK80_t umpk) {
    return inst(umpk)->port5OutGet(); 
}

void UMPK80_Tick(UMPK80_t umpk) {
    inst(umpk)->tick();
}

void UMPK80_Stop(UMPK80_t umpk) {
    inst(umpk)->stop();
}

void UMPK80_Restart(UMPK80_t umpk) {
    inst(umpk)->restart();
}

void UMPK80_KeyboardPressButton(UMPK80_t umpk, uint8_t key) {
    inst(umpk)->pressKey((KeyboardKey)key);
}

void UMPK80_KeyboardReleaseButton(UMPK80_t umpk, uint8_t key) {
    inst(umpk)->releaseKey((KeyboardKey)key);
}

uint8_t UMPK80_DisplayGetDigit(UMPK80_t umpk, int digit) {
    return inst(umpk)->getDisplayDigit(digit);
}

void UMPK80_LoadOS(UMPK80_t umpk, const uint8_t* os) {
    inst(umpk)->loadOS(os);
}

uint16_t UMPK80_CpuProgramCounter(UMPK80_t umpk) {
    return inst(umpk)->getCpu().getProgramCounter();
}

void UMPK80_CpuSetProgramCounter(UMPK80_t umpk, uint16_t value) {
    inst(umpk)->getCpu().setProgramCounter(value);    
}

uint16_t UMPK80_CpuStackPointer(UMPK80_t umpk) {
    return inst(umpk)->getCpu().getStackPointer();
}

uint8_t  UMPK80_CpuGetRegister(UMPK80_t umpk, UMPK80_Register reg) {
    auto& cpu = inst(umpk)->getCpu();
    
    switch (reg) {
        case PC_LOW:  return (cpu.getProgramCounter() & 0xFF);
        case PC_HIGH: return (cpu.getProgramCounter() >> 8) & 0xFF;
        case SP_LOW:  return (cpu.getStackPointer() & 0xFF);
        case SP_HIGH: return (cpu.getStackPointer() >> 8) & 0xFF;
        case L:       return cpu.L();
        case H:       return cpu.H();
        case E:       return cpu.E();
        case D:       return cpu.D();
        case C:       return cpu.C();
        case B:       return cpu.B();
        case A:       return cpu.A(); 
        case PSW:     return cpu.getRegisterFlags();
        case M:       return cpu.getRegister(Cpu::Register::M);
    }
}

uint8_t UMPK80_GetRegister(UMPK80_t umpk, UMPK80_Register reg) {
    return inst(umpk)->getRegister((Umpk80::Register)reg);
}

void UMPK80_SetRegister(UMPK80_t umpk, UMPK80_Register reg, uint8_t value) {
    inst(umpk)->setRegister((Umpk80::Register)reg, value);
}

uint16_t UMPK80_GetRegisterPair(UMPK80_t umpk, UMPK80_RegisterPair regPair) {
    return inst(umpk)->getRegisterPair((Umpk80::RegisterPair)regPair);
}

void UMPK80_SetRegisterPair(UMPK80_t umpk, UMPK80_RegisterPair regPair, uint16_t value) {
    inst(umpk)->setRegisterPair((Umpk80::RegisterPair)regPair, value);
}

uint8_t UMPK80_MemoryRead(UMPK80_t umpk, uint16_t adr) {
    return inst(umpk)->getBus().memoryRead(adr);
}

void UMPK80_MemoryWrite(UMPK80_t umpk, uint16_t adr, uint8_t data) {
    inst(umpk)->getBus().memoryWrite(adr, data);
}

void UMPK80_LoadProgram(UMPK80_t umpk, const uint8_t* program, uint16_t programSize, uint16_t dstAddress) {
    inst(umpk)->getBus().loadRam(program + dstAddress, programSize);
}