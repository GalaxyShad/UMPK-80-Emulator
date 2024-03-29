#include <cstdint>

typedef void* UMPK80_t;

extern "C" {
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

    uint16_t UMPK80_CpuProgramCounter(UMPK80_t umpk);
    uint16_t UMPK80_CpuStackPointer(UMPK80_t umpk);
}

#include "core/umpk80.hpp"

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

uint16_t UMPK80_CpuStackPointer(UMPK80_t umpk) {
    return inst(umpk)->getCpu().getStackPointer();
}