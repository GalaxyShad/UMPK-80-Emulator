#include "cpu.hpp"

#include <assert.h>

Cpu::Cpu(Bus& bus) : _bus(bus) {
    reset();
}

void Cpu::tick() {
    _readCommand();
}


void Cpu::reset() {
    _regFlag.auxcarry = 0;
    _regFlag.carry = 0;
    _regFlag.parry = 0;
    _regFlag.sign = 0;
    _regFlag.zero = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Machine cycles
void Cpu::_readCommand(uint8_t opcode) {
    instructionFunction_t instruction = _instructions[opcode];

    assert(instruction != nullptr);

    _regCmd = opcode;
    _prgCounter++;
    _regAdr = _prgCounter;

    (this->*instruction)();
}

void Cpu::_readCommand() {
    uint8_t opcode = _bus.memoryRead(_prgCounter);

    _readCommand(opcode);
}


void Cpu::_memoryWrite(uint8_t data) {
    _bus.memoryWrite(_regAdr, data);
}


uint8_t Cpu::_memoryRead() {
    uint8_t data = _bus.memoryRead(_regAdr); 

    _prgCounter++;
    _regAdr = _prgCounter;
    
    return data; 
}   


void Cpu::_stackPush(uint16_t data) {
    _stackPointer--;
    _bus.memoryWrite(_stackPointer, (uint8_t)(data >> 8));

    _stackPointer--;
    _bus.memoryWrite(_stackPointer, (uint8_t)(data));
}


uint16_t Cpu::_stackPop() {
    uint16_t data = _bus.memoryRead(_stackPointer);
    _stackPointer++;

    data = (_bus.memoryRead(_stackPointer) << 8) | data;
    _stackPointer++;

    return data;
}


void Cpu::_portWrite(uint8_t port, uint8_t data) {
    _bus.portOut(port, data);
}


uint8_t Cpu::_portRead(uint8_t port) {
    return _bus.portIn(port);
}


// Register operations
uint8_t Cpu::_getRegData(uint8_t regCode) const {
    if (regCode == 0b110) {
        return _bus.memoryRead(((uint16_t)_regH << 8) | _regL);
    }

    return *_registers[regCode];
}


void Cpu::_setRegData(uint8_t regCode, uint8_t data) {
    if (regCode == 0b110) {
        _bus.memoryWrite(((uint16_t)_regH << 8) | _regL, data);
        return;
    }

    *_registers[regCode] = data;
}


uint16_t Cpu::_getRegPairData(uint8_t regPairCode) {
    assert(regPairCode <= 0b11);
    
    uint16_t data = 0x0000;

    switch (regPairCode) {
        case 0b00: data = ((uint16_t)_regB << 8) | _regC; break;
        case 0b01: data = ((uint16_t)_regD << 8) | _regE; break;
        case 0b10: data = ((uint16_t)_regH << 8) | _regL; break;
        case 0b11: data = _stackPointer;                  break;
    }

    return data;
}


void Cpu::_setRegPairData(uint8_t regPairCode, uint16_t data) {
    _setRegPairData(regPairCode, (uint8_t)(data >> 8), (uint8_t)data);
}


void Cpu::_setRegPairData(uint8_t regPairCode, uint8_t dataA, uint8_t dataB) {
    assert(regPairCode <= 0b11);

    switch (regPairCode) {
        case 0b00:  _regB = dataA; _regC = dataB;            break;
        case 0b01:  _regD = dataA; _regE = dataB;            break;
        case 0b10:  _regH = dataA; _regL = dataB;            break;
        case 0b11:  _stackPointer = (dataA << 8) | dataB;    break;
    }
}

// Utility
void Cpu::_updateFlagsState(uint16_t result) {

    uint8_t data = (uint8_t)result;

    _regFlag.carry      = (result >> 8)  ? 0b1 : 0b0;  
    _regFlag.zero       = (data == 0x00) ? 0b1 : 0b0;
    _regFlag.sign       = (data >> 7) & 0b1;

    //https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
    _regFlag.auxcarry   = (result >> 4)  ? 0b1 : 0b0;
   
    uint8_t p = data;
    p ^= p >> 4;
    p ^= p >> 2;
    p ^= p >> 1;
    _regFlag.parry = (~p) & 0b1;
}