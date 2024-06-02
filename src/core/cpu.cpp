#include "cpu.hpp"

Cpu::Cpu(Bus& bus) : _bus(bus) {
    reset();
}

void Cpu::tick() {
    _readCommand();
}

void Cpu::reset() {
    _regFlag.auxcarry = 0;
    _regFlag.carry = 0;
    _regFlag.parity = 0;
    _regFlag.sign = 0;
    _regFlag.zero = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Machine cycles
void Cpu::_readCommand(u8 opcode) {
    instructionFunction_t instruction = _instructions[opcode];

    _regCmd = opcode;
    _prgCounter++;
    _regAdr = _prgCounter;

    (this->*instruction)();
}

void Cpu::_readCommand() {
    u8 opcode = _bus.memoryRead(_prgCounter);

    _readCommand(opcode);
}


void Cpu::_memoryWrite(u8 data) {
    _bus.memoryWrite(_regAdr, data);
}


u8 Cpu::_memoryRead() {
    u8 data = _bus.memoryRead(_regAdr);

    _prgCounter++;
    _regAdr = _prgCounter;
    
    return data; 
}   


void Cpu::_stackPush(u16 data) {
    _bus.memoryWrite(--_stackPointer, (u8)((data >> 8) & 0xFF));
    _bus.memoryWrite(--_stackPointer, (u8)(data & 0xFF));
}


u16 Cpu::_stackPop() {
    u16 data = _bus.memoryRead(_stackPointer++);
    data = (_bus.memoryRead(_stackPointer++) << 8) | data;

    return data;
}


void Cpu::_portWrite(u8 port, u8 data) {
    _bus.portOut(port, data);
}


u8 Cpu::_portRead(u8 port) {
    return _bus.portIn(port);
}

u8 Cpu::_packPsw(CpuFlagsMapping flags) const {
    u8 psw = 0b00000010;

    psw |= flags.sign     << 7;
    psw |= flags.zero     << 6;
    psw |= flags.auxcarry << 4;
    psw |= flags.parity   << 2;
    psw |= flags.carry;

    return psw;
}

CpuFlagsMapping Cpu::_unpackPsw(u8 psw) const {
    CpuFlagsMapping flags;
    
    flags.sign     = (psw & 0b10000000) != 0;
    flags.zero     = (psw & 0b01000000) != 0;
    flags.auxcarry = (psw & 0b00010000) != 0;
    flags.parity   = (psw & 0b00000100) != 0;
    flags.carry    = (psw & 0b00000001) != 0;

    return flags;
}

// Register operations
u8 Cpu::_getRegData(u8 regCode) const {
    if (regCode == 0b110) {
        return _bus.memoryRead(((u16)_regH << 8) | _regL);
    }

    return *_registers[regCode];
}


void Cpu::_setRegData(u8 regCode, u8 data) {
    if (regCode == 0b110) {
        _bus.memoryWrite(((u16)_regH << 8) | _regL, data);
        return;
    }

    *_registers[regCode] = data;
}


u16 Cpu::_getRegPairData(u8 regPairCode) {
    u16 data = 0x0000;

    switch (regPairCode) {
        case 0b00: data = ((u16)_regB << 8) | _regC; break;
        case 0b01: data = ((u16)_regD << 8) | _regE; break;
        case 0b10: data = ((u16)_regH << 8) | _regL; break;
        case 0b11: data = _stackPointer;                  break;
        default: /* do nothing */ break;
    }

    return data;
}


void Cpu::_setRegPairData(u8 regPairCode, u16 data) {
    _setRegPairData(regPairCode, (u8)(data >> 8), (u8)data);
}


void Cpu::_setRegPairData(u8 regPairCode, u8 dataA, u8 dataB) {
    switch (regPairCode) {
        case 0b00:  _regB = dataA; _regC = dataB;            break;
        case 0b01:  _regD = dataA; _regE = dataB;            break;
        case 0b10:  _regH = dataA; _regL = dataB;            break;
        case 0b11:  _stackPointer = (dataA << 8) | dataB;    break;
        default: /* do nothing */ break;
    }
}

// Utility
void Cpu::_updateFlagsState(u16 result) {
    u8 data = (u8)result;

    _regFlag.carry      = (result >> 8)  ? 0b1 : 0b0;  
    _regFlag.zero       = (data == 0x00) ? 0b1 : 0b0;
    _regFlag.sign       = (data >> 7) & 0b1;

    //https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
    _regFlag.auxcarry   = ((data & 0x0F) > 0x09) & 0b1;
   
    u8 p = data;
    p ^= p >> 4;
    p ^= p >> 2;
    p ^= p >> 1;
    _regFlag.parity = (~p) & 0b1;
}