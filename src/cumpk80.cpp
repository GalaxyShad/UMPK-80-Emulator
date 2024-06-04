#include "core/umpk80.hpp"
#include "core/disassembler.hpp"

extern "C" {
    typedef void* UMPK80_t;
    typedef void* UMPK80_I8080Disassembler_t;

    typedef struct UMPK80_Instruction {
        const char* mnemonic;
        int length;
        const char* operand;
    } UMPK80_Instruction_t;

    typedef struct UMPK80_I8080DisassembleResult {
        u16 address;
        u8 bytesCount;
        u8 bytes[3];
        const UMPK80_Instruction_t* instruction;
        bool eof;
    } UMPK80_I8080DisassembleResult_t;

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

    void    UMPK80_PortIOSetInput(UMPK80_t umpk, u8 data);
    u8 UMPK80_PortIOGetInput(UMPK80_t umpk);

    u8 UMPK80_PortIOGetOutput(UMPK80_t umpk);

    void    UMPK80_Tick(UMPK80_t umpk);
    void    UMPK80_Stop(UMPK80_t umpk);
    void    UMPK80_Restart(UMPK80_t umpk);

    void    UMPK80_KeyboardPressButton(UMPK80_t umpk, u8 key);
    void    UMPK80_KeyboardReleaseButton(UMPK80_t umpk, u8 key);

    u8 UMPK80_DisplayGetDigit(UMPK80_t umpk, int digit);
    void    UMPK80_LoadOS(UMPK80_t umpk, const u8* os);

    void    UMPK80_LoadProgram(UMPK80_t umpk, const u8* program, u16 programSize, u16 dstAddress);

    u16 UMPK80_CpuProgramCounter(UMPK80_t umpk);
    void     UMPK80_CpuSetProgramCounter(UMPK80_t umpk, u16 value);
    u16 UMPK80_CpuStackPointer(UMPK80_t umpk);
    u8  UMPK80_CpuGetRegister(UMPK80_t umpk, UMPK80_Register reg);
    void     UMPK80_CpuSetRegister(UMPK80_t umpk, UMPK80_Register reg, u8 data);
    void     UMPK80_CpuJump(UMPK80_t umpk, u16 adr);
    void     UMPK80_CpuCall(UMPK80_t umpk, u16 adr);

    u8 UMPK80_GetRegister(UMPK80_t umpk, UMPK80_Register reg);
    void    UMPK80_SetRegister(UMPK80_t umpk, UMPK80_Register reg, u8 value);

    u16 UMPK80_GetRegisterPair(UMPK80_t umpk, UMPK80_RegisterPair regPair);
    void     UMPK80_SetRegisterPair(UMPK80_t umpk, UMPK80_RegisterPair regPair, u16 value);

    u8 UMPK80_MemoryRead(UMPK80_t umpk, u16 adr);
    void    UMPK80_MemoryWrite(UMPK80_t umpk, u16 adr, u8 data);

    const UMPK80_Instruction_t* UMPK80_GetInstruction(u8 code);

    UMPK80_I8080Disassembler_t UMPK80_CreateI8080Disassembler(const u8* memory, u64 size);
    void                       UMPK80_FreeI8080Disassembler(UMPK80_I8080Disassembler_t disasm);

    UMPK80_I8080DisassembleResult_t UMPK80_I8080DisassemblerDisassemble(UMPK80_I8080Disassembler_t disasm);
    void UMPK80_I8080DisassemblerReset(UMPK80_I8080Disassembler_t disasm);
    u16 UMPK80_I8080DisassemblerPG(UMPK80_I8080Disassembler_t disasm);
}


UMPK80_t UMPK80_Create() {
    return new Umpk80();
}

void UMPK80_Free(UMPK80_t umpk) {
    delete (Umpk80*)umpk;
}

static Umpk80* inst(UMPK80_t umpk) { return (Umpk80*)umpk; }

void UMPK80_PortIOSetInput(UMPK80_t umpk, u8 data) {
    inst(umpk)->port5InSet(data); 
}

u8 UMPK80_PortIOGetInput(UMPK80_t umpk, u8 data) {
    return inst(umpk)->port5InGet(); 
}

u8 UMPK80_PortIOGetOutput(UMPK80_t umpk) {
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

void UMPK80_KeyboardPressButton(UMPK80_t umpk, u8 key) {
    inst(umpk)->pressKey((KeyboardKey)key);
}

void UMPK80_KeyboardReleaseButton(UMPK80_t umpk, u8 key) {
    inst(umpk)->releaseKey((KeyboardKey)key);
}

u8 UMPK80_DisplayGetDigit(UMPK80_t umpk, int digit) {
    return inst(umpk)->getDisplayDigit(digit);
}

void UMPK80_LoadOS(UMPK80_t umpk, const u8* os) {
    inst(umpk)->loadOS(os);
}

u16 UMPK80_CpuProgramCounter(UMPK80_t umpk) {
    return inst(umpk)->getCpu().getProgramCounter();
}

void UMPK80_CpuSetProgramCounter(UMPK80_t umpk, u16 value) {
    inst(umpk)->getCpu().setProgramCounter(value);    
}

u16 UMPK80_CpuStackPointer(UMPK80_t umpk) {
    return inst(umpk)->getCpu().getStackPointer();
}

u8  UMPK80_CpuGetRegister(UMPK80_t umpk, UMPK80_Register reg) {
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

void UMPK80_CpuSetRegister(UMPK80_t umpk, UMPK80_Register reg, u8 data) {
    auto& cpu = inst(umpk)->getCpu();

    switch (reg) {
        case PC_LOW:  cpu.setProgramCounter((cpu.getProgramCounter() & 0xFF00) | data);
        case PC_HIGH: cpu.setProgramCounter((cpu.getProgramCounter() & 0x00FF) | (((u16)data) << 8));
        case SP_LOW:  cpu.setStackPointer((cpu.getStackPointer() & 0xFF00) | data);
        case SP_HIGH: cpu.setStackPointer((cpu.getStackPointer() & 0x00FF) | (((u16)data) << 8));
        case L:       cpu.setRegister(Cpu::Register::L, data);
        case H:       cpu.setRegister(Cpu::Register::H, data);
        case E:       cpu.setRegister(Cpu::Register::E, data);
        case D:       cpu.setRegister(Cpu::Register::D, data);
        case C:       cpu.setRegister(Cpu::Register::C, data);
        case B:       cpu.setRegister(Cpu::Register::B, data);
        case A:       cpu.setRegister(Cpu::Register::A, data);
        case PSW:     cpu.setRegisterFlags(data);
        case M:       cpu.setRegister(Cpu::Register::M, data);
    }
}

void UMPK80_CpuJump(UMPK80_t umpk, u16 adr) {
    inst(umpk)->getCpu().forceJump(adr);
}

void UMPK80_CpuCall(UMPK80_t umpk, u16 adr) {
    inst(umpk)->getCpu().forceCall(adr);
}

u8 UMPK80_GetRegister(UMPK80_t umpk, UMPK80_Register reg) {
    return inst(umpk)->getRegister((Umpk80::Register)reg);
}

void UMPK80_SetRegister(UMPK80_t umpk, UMPK80_Register reg, u8 value) {
    inst(umpk)->setRegister((Umpk80::Register)reg, value);
}

u16 UMPK80_GetRegisterPair(UMPK80_t umpk, UMPK80_RegisterPair regPair) {
    return inst(umpk)->getRegisterPair((Umpk80::RegisterPair)regPair);
}

void UMPK80_SetRegisterPair(UMPK80_t umpk, UMPK80_RegisterPair regPair, u16 value) {
    inst(umpk)->setRegisterPair((Umpk80::RegisterPair)regPair, value);
}

u8 UMPK80_MemoryRead(UMPK80_t umpk, u16 adr) {
    return inst(umpk)->getBus().memoryRead(adr);
}

void UMPK80_MemoryWrite(UMPK80_t umpk, u16 adr, u8 data) {
    inst(umpk)->getBus().memoryWrite(adr, data);
}

void UMPK80_LoadProgram(UMPK80_t umpk, const u8* program, u16 programSize, u16 dstAddress) {
    inst(umpk)->getBus().loadRam(program, programSize, dstAddress);
}

const UMPK80_Instruction_t* UMPK80_GetInstruction(u8 code) {
    return (UMPK80_Instruction_t*) &Disassembler::getInstruction(code);
}

UMPK80_I8080Disassembler_t UMPK80_CreateI8080Disassembler(const u8 *memory,
                                                          u64 size) {
    return (UMPK80_I8080Disassembler_t) new Disassembler(memory, size);
}

void UMPK80_FreeI8080Disassembler(UMPK80_I8080Disassembler_t disasm) {
    delete (Disassembler*)disasm;
}

UMPK80_I8080DisassembleResult_t
UMPK80_I8080DisassemblerDisassemble(UMPK80_I8080Disassembler_t disasm) {
    auto* dis = (Disassembler*)disasm;

    auto res = dis->disassemble();

    return *((UMPK80_I8080DisassembleResult_t*)&res);
}

void UMPK80_I8080DisassemblerReset(UMPK80_I8080Disassembler_t disasm) {
    ((Disassembler*)disasm)->reset();
}

u16 UMPK80_I8080DisassemblerPG(UMPK80_I8080Disassembler_t disasm) {
    return ((Disassembler*)disasm)->getPgCounter();
}
