#include "cpu.hpp"

#include <exception>
#include <iostream>

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
void Cpu::_readCommand() {
    uint8_t opcode = _bus.read(_prgCounter);

    instructionFunction_t instruction = _instructions[opcode];

    if (instruction == nullptr) {
        printf("%x\n", _prgCounter);
        throw std::logic_error(std::to_string(opcode) + " Unexisting command");
    }

    _regCmd = opcode;
    _prgCounter++;
    _regAdr = _prgCounter;

    (this->*instruction)();
}


void Cpu::_memoryWrite(uint8_t data) {
    _bus.write(_regAdr, data);
}


uint8_t Cpu::_memoryRead() {
    uint8_t data = _bus.read(_regAdr); 

    _prgCounter++;
    _regAdr = _prgCounter;
    
    return data; 
}   


void Cpu::_stackPush(uint16_t data) {
    _bus.write(_stackPointer, (uint8_t)data);
    _bus.write(_stackPointer, (uint8_t)(data >> 8));

    _stackPointer -= 2;
}


uint16_t Cpu::_stackPop() {
    uint16_t data = _bus.read(_stackPointer);
    _stackPointer++;

    data = (data << 8) | _bus.read(_stackPointer);
    _stackPointer++;

    return data;
}


void Cpu::_portWrite(uint8_t port, uint8_t data) {
    throw std::logic_error("Not implemented");
}


uint8_t Cpu::_portRead(uint8_t port) {
    throw std::logic_error("Not implemented");
}


// Register operations
uint8_t Cpu::_getRegData(uint8_t regCode) {
    if (regCode == 0b110) {
        return _bus.read(((uint16_t)_regH << 8) | _regL);
    }

    return *_registers[regCode];
}


void Cpu::_setRegData(uint8_t regCode, uint8_t data) {
    if (regCode == 0b110) {
        _bus.write(((uint16_t)_regH << 8) | _regL, data);
        return;
    }

    *_registers[regCode] = data;
}


uint16_t Cpu::_getRegPairData(uint8_t regPairCode) {
    uint16_t data = 0x0000;

    switch (regPairCode) {
        case 0b00: data = (_regB << 8) | _regC; break;
        case 0b01: data = (_regD << 8) | _regE; break;
        case 0b10: data = (_regH << 8) | _regL; break;
        case 0b11: data = _stackPointer;        break;
        default:   throw std::logic_error("Unexisting register pair code");
    }

    return data;
}


void Cpu::_setRegPairData(uint8_t regPairCode, uint16_t data) {
    _setRegPairData(regPairCode, (uint8_t)(data >> 8), (uint8_t)data);
}


void Cpu::_setRegPairData(uint8_t regPairCode, uint8_t dataA, uint8_t dataB) {
    switch (regPairCode) {
        case 0b00:  _regB = dataA; _regC = dataB;            break;
        case 0b01:  _regD = dataA; _regE = dataB;            break;
        case 0b10:  _regH = dataA; _regL = dataB;            break;
        case 0b11:  _stackPointer = (dataA << 8) | dataB;    break;
        default:    throw std::logic_error("Unexisting register pair code");
    }

}

// Utility
void Cpu::_updateFlagsState(uint16_t result) {

    uint8_t data = (uint8_t)result;

    _regFlag.carry      = (result >> 8)  ? 0b1 : 0b0;  
    _regFlag.zero       = (data == 0x00) ? 0b1 : 0b0;
    _regFlag.sign       = (data >> 7) & 0b1;
    _regFlag.auxcarry   = (result >> 4)  ? 0b1 : 0b0;
    
    uint8_t p = data;
    p ^= p >> 4;
    p ^= p >> 2;
    p ^= p >> 1;
    _regFlag.parry = (~p) & 0b1;
}

// Nop instruction
void Cpu::_nop() { 

}

// Carry bit instructions
void Cpu::_stc() { 
    _regFlag.carry = 0b1;
}
void Cpu::_cmc() { 
    _regFlag.carry ^= 0b1;
}

// Single register instructions
void Cpu::_inr() { 
    uint8_t regCode = (_regCmd & 0b00111000) >> 3;

    uint16_t data = _getRegData(regCode);
    data++;

    uint8_t tc = _regFlag.carry;
    _updateFlagsState(data);
    _regFlag.carry = tc;
    _setRegData(regCode, data);
}


void Cpu::_dcr() { 
    uint8_t regCode = (_regCmd & 0b00111000) >> 3;

    uint16_t data = _getRegData(regCode);
    data--;

    uint8_t tc = _regFlag.carry;
    _updateFlagsState(data);
    _regFlag.carry = tc;
    _setRegData(regCode, data);
}


void Cpu::_cma() { 
    _regA = ~_regA;
}


void Cpu::_daa() { 
    throw std::logic_error("Not implemented"); 
}


// Data transfer instructions
void Cpu::_mov() { 
    uint8_t srcReg = (_regCmd & 0b00000111);
    uint8_t dstReg = (_regCmd & 0b00111000) >> 3;

    uint16_t srcData = _getRegData(srcReg);

    _setRegData(dstReg, srcData);
}


void Cpu::_stax() { 
    uint8_t rp = (_regCmd & 0b00010000) >> 4;

    uint16_t adr = _getRegPairData(rp);

    _bus.write(adr, _regA);
}


void Cpu::_ldax() { 
    uint8_t rp = (_regCmd & 0b00010000) >> 4;

    uint16_t adr = _getRegPairData(rp);

    _regA = _bus.read(adr);
}


// Arithmetical or logical instructions
void Cpu::_add() { 
    uint8_t regCode = (_regCmd & 0b111);

    uint16_t res = _regA;
    res += _getRegData(regCode);

    _updateFlagsState(res);
    _regA = (uint8_t)res;
}


void Cpu::_adc() { 
    uint8_t regCode = (_regCmd & 0b111);

    uint16_t res = _regA;
    res += _getRegData(regCode) + _regFlag.carry;

    _updateFlagsState(res);
    _regA = (uint8_t)res;
}


void Cpu::_sub() { 
    uint8_t regCode = (_regCmd & 0b111);

    uint16_t res = _regA;
    res -= _getRegData(regCode);

    _updateFlagsState(res);
    _regA = (uint8_t)res;
}


void Cpu::_sbb() { 
    uint8_t regCode = (_regCmd & 0b111);

    uint16_t res = _regA;
    res -= _getRegData(regCode) - _regFlag.carry;

    _updateFlagsState(res);
    _regA = (uint8_t)res;
}


void Cpu::_ana() { 
    uint8_t regCode = (_regCmd & 0b111);

    uint16_t res = _regA;
    res &= _getRegData(regCode);

    uint8_t ac = _regFlag.auxcarry;
    _updateFlagsState(res);
    _regFlag.auxcarry = ac;
    _regFlag.carry = 0b0;

    _regA = (uint8_t)res;
}


void Cpu::_xra() { 
    uint8_t regCode = (_regCmd & 0b111);

    uint16_t res = _regA;
    res ^= _getRegData(regCode);

    _updateFlagsState(res);
    _regFlag.auxcarry   = 0b0;
    _regFlag.carry      = 0b0;

    _regA = (uint8_t)res;
}


void Cpu::_ora() { 
    uint8_t regCode = (_regCmd & 0b111);

    uint16_t res = _regA;
    res |= _getRegData(regCode);

    uint8_t ac = _regFlag.auxcarry;
    _updateFlagsState(res);
    _regFlag.auxcarry = ac;
    _regFlag.carry = 0b0;

    _regA = (uint8_t)res;
}


void Cpu::_cmp() { 
    uint8_t regCode = (_regCmd & 0b111);

    uint16_t res = _regA;
    res -= _getRegData(regCode);

    _updateFlagsState(res);
}


// Immediate instructions
void Cpu::_lxi() { 
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    uint16_t adr = _memoryRead() | (_memoryRead() << 8);

    _setRegPairData(regPairCode, adr);
}


void Cpu::_mvi() { 
    uint8_t regCode = (_regCmd & 0b00111000) >> 3;

    uint8_t data = _memoryRead();

    _setRegData(regCode, data);
}


void Cpu::_adi() { 
    uint16_t res = _regA;
    res += _memoryRead();

    _updateFlagsState(res);
    _regA = (uint8_t)res;
}


void Cpu::_aci() { 
    uint16_t res = _regA;
    res += _memoryRead() + _regFlag.carry;

    _updateFlagsState(res);
    _regA = (uint8_t)res;
}


void Cpu::_sui() { 
    uint16_t res = _regA;
    res -= _memoryRead();

    _updateFlagsState(res);
    _regA = (uint8_t)res;
}


void Cpu::_sbi() { 
    uint16_t res = _regA;
    res -= _memoryRead() - _regFlag.carry;

    _updateFlagsState(res);
    _regA = (uint8_t)res;
}


void Cpu::_ani() { 
    uint16_t res = _regA;
    res &= _memoryRead();

    uint8_t ac = _regFlag.auxcarry;
    _updateFlagsState(res);
    _regFlag.auxcarry = ac;
    _regFlag.carry = 0b0;

    _regA = (uint8_t)res;
}


void Cpu::_xri() { 
    uint16_t res = _regA;
    res ^= _memoryRead();

    _updateFlagsState(res);
    _regFlag.auxcarry   = 0b0;
    _regFlag.carry      = 0b0;

    _regA = (uint8_t)res;
}


void Cpu::_ori() { 
    uint16_t res = _regA;
    res |= _memoryRead();

    uint8_t ac = _regFlag.auxcarry;
    _updateFlagsState(res);
    _regFlag.auxcarry = ac;
    _regFlag.carry = 0b0;

    _regA = (uint8_t)res;
}


void Cpu::_cpi() { 
    uint16_t res = _regA;
    res -= _memoryRead();

    _updateFlagsState(res);
}

// Rotate accumulator instructions
void Cpu::_rlc() { 
    _regFlag.carry = (_regA & 0b10000000) >> 7;
    _regA = (_regA << 1) | _regFlag.carry;
}


void Cpu::_rrc() { 
    _regFlag.carry = _regA & 0b1;
    _regA = (_regA >> 1) | (_regFlag.carry << 7);
}


void Cpu::_ral() { 
    uint8_t tempCarry = _regFlag.carry;
    _regFlag.carry = (_regA & 0b10000000) >> 7;
    _regA = (_regA << 1) | tempCarry;
}


void Cpu::_rar() { 
    uint8_t tempCarry = _regFlag.carry;
    _regFlag.carry = (_regA & 0b1);
    _regA = (_regA << 1) | (tempCarry << 7);
}

// Register pair instructions
void Cpu::_push() {
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    // Flags and A store
    if (regPairCode == 0b11) {
        uint8_t  flags = *((uint8_t*)&_regFlag); 
        uint16_t fa = (flags << 8) | _regA;

        _stackPush(fa);

        return;
    }
    
    uint16_t data = _getRegPairData(regPairCode);
    _stackPush(data);
}


void Cpu::_pop() { 
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    uint16_t data = _stackPop();

    // Flags and A read
    if (regPairCode == 0b11) {
        *((uint8_t*)&_regFlag) = data >> 8;
        _regA = (uint8_t)data;

        return;
    }
    
    _setRegPairData(regPairCode, data);
}


void Cpu::_dad() { 
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    uint16_t data = _getRegPairData(regPairCode);
    uint16_t hl   = _getRegPairData(0b10);

    uint32_t res  = data + hl;

    _regFlag.carry = res & 0xFFFF0000;
}


void Cpu::_inx() { 
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    uint16_t data = _getRegPairData(regPairCode);
    data++;

    _setRegPairData(regPairCode, data);
}


void Cpu::_dcx() { 
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    uint16_t data = _getRegPairData(regPairCode);
    data--;

    _setRegPairData(regPairCode, data);
}


void Cpu::_xchg() {
    uint8_t x = _regH;
    uint8_t y = _regL;

    _regH = _regD;
    _regL = _regE;

    _regD = x;
    _regE = y;
}


void Cpu::_xthl() { 

}


void Cpu::_sphl() { 

}

// Direct adressing instructions
void Cpu::_sta() { 
    uint8_t  lowAdr = _memoryRead();
    _regAdr = (_memoryRead() << 8) | lowAdr;

    _memoryWrite(_regA);
}

void Cpu::_lda() { 
    uint8_t  lowAdr = _memoryRead();
    _regAdr = (_memoryRead() << 8) | lowAdr;

    _regA = _bus.read(_regAdr);
}


void Cpu::_shld() { throw std::logic_error("Not implemented"); }
void Cpu::_lhld() { throw std::logic_error("Not implemented"); }

// Jump instructions
void Cpu::_pchl() { throw std::logic_error("Not implemented"); }


void Cpu::_jmp(bool cond) { 
    uint8_t  lowAdr = _memoryRead();
    uint16_t adr    = (_memoryRead() << 8) | lowAdr;
    
    if (!cond) return;

    _prgCounter = adr;
    
}

void Cpu::_jmp()    { _jmp(true);               }
void Cpu::_jc()     { _jmp(_regFlag.carry);     }
void Cpu::_jnc()    { _jmp(!_regFlag.carry);    }
void Cpu::_jz()     { _jmp(_regFlag.zero);      }
void Cpu::_jnz()    { _jmp(!_regFlag.zero);     }
void Cpu::_jm()     { _jmp(_regFlag.sign);      }
void Cpu::_jp()     { _jmp(!_regFlag.sign);     }
void Cpu::_jpe()    { _jmp(_regFlag.parry);     }
void Cpu::_jpo()    { _jmp(!_regFlag.parry);    }

// Call instructions
void Cpu::_call(bool cond) { 
    uint8_t  lowAdr = _memoryRead();
    uint16_t adr    = (_memoryRead() << 8) | lowAdr;
    
    if (!cond) return;

    _stackPush(_prgCounter);
    _prgCounter = adr;
}

void Cpu::_call()   { _call(true);              }
void Cpu::_cc()     { _call(_regFlag.carry);    }
void Cpu::_cnc()    { _call(!_regFlag.carry);   }
void Cpu::_cz()     { _call(_regFlag.zero);     }
void Cpu::_cnz()    { _call(!_regFlag.zero);    }
void Cpu::_cm()     { _call(_regFlag.sign);     }
void Cpu::_cp()     { _call(!_regFlag.sign);    }
void Cpu::_cpe()    { _call(_regFlag.parry);    }
void Cpu::_cpo()    { _call(!_regFlag.parry);   }

// Return instructions 
void Cpu::_ret(bool cond) { 
    if (!cond) return;

    uint16_t adr    = _stackPop();
    _prgCounter = adr;
}

void Cpu::_ret() { _ret(true);              }
void Cpu::_rc()  { _ret(_regFlag.carry);    }
void Cpu::_rnc() { _ret(!_regFlag.carry);   }
void Cpu::_rz()  { _ret(_regFlag.zero);     }
void Cpu::_rnz() { _ret(!_regFlag.zero);    }
void Cpu::_rm()  { _ret(_regFlag.sign);     }
void Cpu::_rp()  { _ret(!_regFlag.sign);    }
void Cpu::_rpe() { _ret(_regFlag.parry);    }
void Cpu::_rpo() { _ret(!_regFlag.parry);   }

// Rst instruction
void Cpu::_rst() { 
    uint8_t rstCode = (_regCmd & 0b00111000) >> 3;

    _stackPush(_prgCounter);

    _prgCounter = rstCode << 3;
}

// Interrupt Flip-Flop instructions
void Cpu::_ei() { throw std::logic_error("Not implemented"); }
void Cpu::_di() { throw std::logic_error("Not implemented"); }

// IO instructions
void Cpu::_in()  { 
    uint8_t port = _memoryRead();
    printf("PORT = 0x%02x data required >> ", port);

    scanf("%x", &_regA);
    // throw std::logic_error("Not implemented"); 
}
void Cpu::_out() { 
    uint8_t port = _memoryRead();
    printf("PORT = 0x%02x OUT = 0x%02x\n", port, _regA);
    // throw std::logic_error("Not implemented"); 
}

// Hlt instruction
void Cpu::_hlt() { _hold = true; }