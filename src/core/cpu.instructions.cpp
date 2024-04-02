#include "cpu.hpp"

#include <assert.h>

// Nop instruction
void Cpu::_nop() {}

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
    uint8_t aLow  = _regA & 0x0F; 
    
    uint16_t res = _regA;

    if (aLow > 0x09 || _regFlag.auxcarry) {
        res += 0x06;
        _updateFlagsState(res);

        _regA = (uint8_t)res;
    }

    uint8_t aHigh = (_regA & 0xF0) >> 4;

    if (aHigh > 0x09 || _regFlag.carry) {
        aHigh += 0x06;
        _regA = (aHigh << 4) | (_regA & 0x0F);
    }
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

    _bus.memoryWrite(adr, _regA);
}


void Cpu::_ldax() { 
    uint8_t rp = (_regCmd & 0b00010000) >> 4;

    uint16_t adr = _getRegPairData(rp);

    _regA = _bus.memoryRead(adr);
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

    uint8_t  lowAdr = _memoryRead();
    uint16_t adr    = (_memoryRead() << 8) | lowAdr;

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

#pragma endregion

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
    _regA = (_regA >> 1) | (tempCarry << 7);
}

// Register pair instructions
void Cpu::_push() {
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    // Flags and A store
    if (regPairCode == 0b11) {
        uint8_t psw = 0b00000010;

        psw |= _regFlag.sign     << 7;
        psw |= _regFlag.zero     << 6;
        psw |= _regFlag.auxcarry << 4;
        psw |= _regFlag.parity   << 2;
        psw |= _regFlag.carry;

        uint16_t af =  ((uint16_t)_regA << 8) | psw;

        _stackPush(af);

        return;
    }
    
    uint16_t data = _getRegPairData(regPairCode);
    _stackPush(data);
}


void Cpu::_pop() { 
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    uint16_t apsw = _stackPop();

    // Flags and A read
    if (regPairCode == 0b11) {
        _regFlag.sign     = (apsw & 0b10000000) != 0;
        _regFlag.zero     = (apsw & 0b01000000) != 0;
        _regFlag.auxcarry = (apsw & 0b00010000) != 0;
        _regFlag.parity   = (apsw & 0b00000100) != 0;
        _regFlag.carry    = (apsw & 0b00000001) != 0;

        _regA = apsw >> 8;

        return;
    }
    
    _setRegPairData(regPairCode, apsw);
}


void Cpu::_dad() { 
    uint8_t regPairCode = (_regCmd & 0b00110000) >> 4;

    uint16_t data = _getRegPairData(regPairCode);
    uint16_t hl   = _getRegPairData(0b10);

    uint32_t res  = data + hl;

    _regFlag.carry = ((res & 0xFFFF0000) != false) & 0b1;

    _setRegPairData(0b10, (uint16_t)res);
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
    uint8_t h = _regH;
    uint8_t l = _regL;

    _regH = _regD;
    _regL = _regE;

    _regD = h;
    _regE = l;
}


void Cpu::_xthl() { 
    uint8_t h = _regH;
    uint8_t l = _regL;

    _regL = _bus.memoryRead(_stackPointer);
    _regH = _bus.memoryRead(_stackPointer+1);

    _bus.memoryWrite(_stackPointer,   l);
    _bus.memoryWrite(_stackPointer+1, h);
}


void Cpu::_sphl() { 
    _stackPointer = (_regH << 8) | _regL;
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

    _regA = _bus.memoryRead(_regAdr);
}


void Cpu::_shld() { 
    uint8_t  lowAdr = _memoryRead();
    uint16_t adr    = ((uint16_t)_memoryRead() << 8) | lowAdr;

    _bus.memoryWrite(adr,   _regL);
    _bus.memoryWrite(adr+1, _regH);
}


void Cpu::_lhld() { 
    uint8_t  lowAdr = _memoryRead();
    uint16_t adr    = (_memoryRead() << 8) | lowAdr;

    _regL = _bus.memoryRead(adr);
    _regH = _bus.memoryRead(adr+1);
}

// Jump instructions
void Cpu::_pchl() { 
    _prgCounter = (_regH << 8) | _regL;
}


void Cpu::_jmp(bool cond) { 
    uint8_t  lowAdr = _memoryRead();
    uint16_t adr    = (_memoryRead() << 8) | lowAdr;
    
    if (!cond) return;

    _prgCounter = adr;
    
}

void Cpu::_jmp()    { _jmp(true);                  }
void Cpu::_jc()     { _jmp(_regFlag.carry == 0b1); }
void Cpu::_jnc()    { _jmp(_regFlag.carry == 0b0); }
void Cpu::_jz()     { _jmp(_regFlag.zero  == 0b1); }
void Cpu::_jnz()    { _jmp(_regFlag.zero  == 0b0); }
void Cpu::_jm()     { _jmp(_regFlag.sign  == 0b1); }
void Cpu::_jp()     { _jmp(_regFlag.sign  == 0b0); }
void Cpu::_jpe()    { _jmp(_regFlag.parity == 0b1); }
void Cpu::_jpo()    { _jmp(_regFlag.parity == 0b0); }

// Call instructions
void Cpu::_call(uint16_t adr, bool cond) {
    if (!cond) return;

    _stackPush(_prgCounter);
    _prgCounter = adr;
}

void Cpu::_call(bool cond) { 
    uint8_t  lowAdr = _memoryRead();
    uint16_t adr    = (_memoryRead() << 8) | lowAdr;
    
    _call(adr, cond);
}

void Cpu::_call()   { _call(true);                  }
void Cpu::_cc()     { _call(_regFlag.carry == 0b1); }
void Cpu::_cnc()    { _call(_regFlag.carry == 0b0); }
void Cpu::_cz()     { _call(_regFlag.zero  == 0b1); }
void Cpu::_cnz()    { _call(_regFlag.zero  == 0b0); }
void Cpu::_cm()     { _call(_regFlag.sign  == 0b1); }
void Cpu::_cp()     { _call(_regFlag.sign  == 0b0); }
void Cpu::_cpe()    { _call(_regFlag.parity == 0b1); }
void Cpu::_cpo()    { _call(_regFlag.parity == 0b0); }

// Return instructions 
void Cpu::_ret(bool cond) { 
    if (!cond) return;

    uint16_t adr    = _stackPop();
    _prgCounter = adr;
}

void Cpu::_ret() { _ret(true);                  }
void Cpu::_rc()  { _ret(_regFlag.carry == 0b1); }
void Cpu::_rnc() { _ret(_regFlag.carry == 0b0); }
void Cpu::_rz()  { _ret(_regFlag.zero  == 0b1); }
void Cpu::_rnz() { _ret(_regFlag.zero  == 0b0); }
void Cpu::_rm()  { _ret(_regFlag.sign  == 0b1); }
void Cpu::_rp()  { _ret(_regFlag.sign  == 0b0); }
void Cpu::_rpe() { _ret(_regFlag.parity == 0b1); }
void Cpu::_rpo() { _ret(_regFlag.parity == 0b0); }

// Rst instruction
void Cpu::_rst() { 
    uint8_t rstCode = (_regCmd & 0b00111000) >> 3;

    _stackPush(_prgCounter);

    _prgCounter = rstCode << 3;
}

// Interrupt Flip-Flop instructions
void Cpu::_ei() { _interruptsEnabled = true; }
void Cpu::_di() { _interruptsEnabled = false; }

// IO instructions
void Cpu::_in()  { 
    uint8_t port = _memoryRead();
    _regA = _portRead(port);
}

void Cpu::_out() { 
    uint8_t port = _memoryRead();
    _portWrite(port, _regA);
}

// Hlt instruction
void Cpu::_hlt() { _hold = true; }