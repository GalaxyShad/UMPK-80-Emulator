
#include "int-defines.hpp"

typedef void* UMPK80_t;

extern "C" {
    int hello();

    UMPK80_t UMPK80_Create();
    void     UMPK80_Free(UMPK80_t umpk);

    void    UMPK80_PortIO_In_Set(UMPK80_t umpk, uint8_t data);
    uint8_t UMPK80_PortIO_In_Get(UMPK80_t umpk);

    uint8_t UMPK80_PortIO_Out_Get(UMPK80_t umpk);

    void    UMPK80_Tick(UMPK80_t umpk);
    void    UMPK80_Stop(UMPK80_t umpk);
    void    UMPK80_Restart(UMPK80_t umpk);

    void    UMPK80_PressKey(UMPK80_t umpk);
    void    UMPK80_ReleaseKey(UMPK80_t umpk);

    uint8_t UMPK80_GetDisplayDigit(UMPK80_t umpk, int digit);
    void    UMPK80_LoadOS(UMPK80_t umpk, const uint8_t* os);

    uint16_t UMPK80_Cpu_GetProgramCounter(UMPK80_t umpk);
}

#include "umpk80.hpp"

int hello() {
    return 8;
}

UMPK80_t UMPK80_Create() {
    return new Umpk80();
}

void UMPK80_Free(UMPK80_t umpk) {
    delete umpk;
}

void UMPK80_PortIO_In_Set(UMPK80_t umpk, uint8_t data) {
    ((Umpk80*)umpk)->port5InSet(data); 
}

uint8_t UMPK80_PortIO_In_Get(UMPK80_t umpk, uint8_t data) {
    return ((Umpk80*)umpk)->port5InGet(); 
}

uint8_t UMPK80_PortIO_Out_Get(UMPK80_t umpk) {
    return ((Umpk80*)umpk)->port5OutGet(); 
}

void UMPK80_Tick(UMPK80_t umpk) {
    ((Umpk80*)umpk)->tick();
}

void UMPK80_Stop(UMPK80_t umpk) {
    ((Umpk80*)umpk)->stop();
}

void UMPK80_Restart(UMPK80_t umpk) {
    ((Umpk80*)umpk)->restart();
}

void UMPK80_PressKey(UMPK80_t umpk) {
    ((Umpk80*)umpk)->pressKey(Keyboard::Key::_5);
}

void UMPK80_ReleaseKey(UMPK80_t umpk) {
    ((Umpk80*)umpk)->releaseKey(Keyboard::Key::_5);
}

uint8_t UMPK80_GetDisplayDigit(UMPK80_t umpk, int digit) {
    return ((Umpk80*)umpk)->getDisplayDigit(digit);
}

void UMPK80_LoadOS(UMPK80_t umpk, const uint8_t* os) {
    ((Umpk80*)umpk)->loadOS(os);
}

uint16_t UMPK80_Cpu_GetProgramCounter(UMPK80_t umpk) {
    return ((Umpk80*)umpk)->getCpu().getProgramCounter();
}