#include "cpu.hpp"

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
    u8 regCode = (_regCmd & 0b00111000) >> 3;

    u16 data = _getRegData(regCode);
    data++;

    u8 tc = _regFlag.carry;
    _updateFlagsState(data);
    _regFlag.carry = tc;
    _setRegData(regCode, data);
}


void Cpu::_dcr() { 
    u8 regCode = (_regCmd & 0b00111000) >> 3;

    u16 data = _getRegData(regCode);
    data--;

    u8 tc = _regFlag.carry;
    _updateFlagsState(data);
    _regFlag.carry = tc;
    _setRegData(regCode, data);
}


void Cpu::_cma() { 
    _regA = ~_regA;
}


void Cpu::_daa() { 
    u8 aLow  = _regA & 0x0F;
    
    u16 res = _regA;

    if (aLow > 0x09 || _regFlag.auxcarry) {
        res += 0x06;
        _updateFlagsState(res);

        _regA = (u8)res;
    }

    u8 aHigh = (_regA & 0xF0) >> 4;

    if (aHigh > 0x09 || _regFlag.carry) {
        aHigh += 0x06;
        _regA = (aHigh << 4) | (_regA & 0x0F);
    }
}


// Data transfer instructions
void Cpu::_mov() { 
    u8 srcReg = (_regCmd & 0b00000111);
    u8 dstReg = (_regCmd & 0b00111000) >> 3;

    u16 srcData = _getRegData(srcReg);

    _setRegData(dstReg, srcData);
}


void Cpu::_stax() { 
    u8 rp = (_regCmd & 0b00010000) >> 4;

    u16 adr = _getRegPairData(rp);

    _bus.memoryWrite(adr, _regA);
}


void Cpu::_ldax() { 
    u8 rp = (_regCmd & 0b00010000) >> 4;

    u16 adr = _getRegPairData(rp);

    _regA = _bus.memoryRead(adr);
}


// Arithmetical or logical instructions
void Cpu::_add() { 
    u8 regCode = (_regCmd & 0b111);

    u16 res = _regA;
    res += _getRegData(regCode);

    _updateFlagsState(res);
    _regA = (u8)res;
}


void Cpu::_adc() { 
    u8 regCode = (_regCmd & 0b111);

    u16 res = _regA;
    res += _getRegData(regCode) + _regFlag.carry;

    _updateFlagsState(res);
    _regA = (u8)res;
}


void Cpu::_sub() { 
    u8 regCode = (_regCmd & 0b111);

    u16 res = _regA;
    res -= _getRegData(regCode);

    _updateFlagsState(res);
    _regA = (u8)res;
}


void Cpu::_sbb() { 
    u8 regCode = (_regCmd & 0b111);

    u16 res = _regA;
    res -= _getRegData(regCode) - _regFlag.carry;

    _updateFlagsState(res);
    _regA = (u8)res;
}


void Cpu::_ana() { 
    u8 regCode = (_regCmd & 0b111);

    u16 res = _regA;
    res &= _getRegData(regCode);

    u8 ac = _regFlag.auxcarry;
    _updateFlagsState(res);
    _regFlag.auxcarry = ac;
    _regFlag.carry = 0b0;

    _regA = (u8)res;
}


void Cpu::_xra() { 
    u8 regCode = (_regCmd & 0b111);

    u16 res = _regA;
    res ^= _getRegData(regCode);

    _updateFlagsState(res);
    _regFlag.auxcarry   = 0b0;
    _regFlag.carry      = 0b0;

    _regA = (u8)res;
}


void Cpu::_ora() { 
    u8 regCode = (_regCmd & 0b111);

    u16 res = _regA;
    res |= _getRegData(regCode);

    u8 ac = _regFlag.auxcarry;
    _updateFlagsState(res);
    _regFlag.auxcarry = ac;
    _regFlag.carry = 0b0;

    _regA = (u8)res;
}


void Cpu::_cmp() { 
    u8 regCode = (_regCmd & 0b111);

    u16 res = _regA;
    res -= _getRegData(regCode);

    _updateFlagsState(res);
}


// Immediate instructions
void Cpu::_lxi() { 
    u8 regPairCode = (_regCmd & 0b00110000) >> 4;

    u8  lowAdr = _memoryRead();
    u16 adr    = (_memoryRead() << 8) | lowAdr;

    _setRegPairData(regPairCode, adr);
}


void Cpu::_mvi() { 
    u8 regCode = (_regCmd & 0b00111000) >> 3;

    u8 data = _memoryRead();

    _setRegData(regCode, data);
}


void Cpu::_adi() { 
    u16 res = _regA;
    res += _memoryRead();

    _updateFlagsState(res);
    _regA = (u8)res;
}


void Cpu::_aci() { 
    u16 res = _regA;
    res += _memoryRead() + _regFlag.carry;

    _updateFlagsState(res);
    _regA = (u8)res;
}


void Cpu::_sui() { 
    u16 res = _regA;
    res -= _memoryRead();

    _updateFlagsState(res);
    _regA = (u8)res;
}


void Cpu::_sbi() { 
    u16 res = _regA;
    res -= _memoryRead() - _regFlag.carry;

    _updateFlagsState(res);
    _regA = (u8)res;
}


void Cpu::_ani() { 
    u16 res = _regA;
    res &= _memoryRead();

    u8 ac = _regFlag.auxcarry;
    _updateFlagsState(res);
    _regFlag.auxcarry = ac;
    _regFlag.carry = 0b0;

    _regA = (u8)res;
}


void Cpu::_xri() { 
    u16 res = _regA;
    res ^= _memoryRead();

    _updateFlagsState(res);
    _regFlag.auxcarry   = 0b0;
    _regFlag.carry      = 0b0;

    _regA = (u8)res;
}


void Cpu::_ori() { 
    u16 res = _regA;
    res |= _memoryRead();

    u8 ac = _regFlag.auxcarry;
    _updateFlagsState(res);
    _regFlag.auxcarry = ac;
    _regFlag.carry = 0b0;

    _regA = (u8)res;
}


void Cpu::_cpi() { 
    u16 res = _regA;
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
    u8 tempCarry = _regFlag.carry;
    _regFlag.carry = (_regA & 0b10000000) >> 7;
    _regA = (_regA << 1) | tempCarry;
}


void Cpu::_rar() { 
    u8 tempCarry = _regFlag.carry;
    _regFlag.carry = (_regA & 0b1);
    _regA = (_regA >> 1) | (tempCarry << 7);
}

// Register pair instructions
void Cpu::_push() {
    u8 regPairCode = (_regCmd & 0b00110000) >> 4;

    // Flags and A store
    if (regPairCode == 0b11) {
        u8 psw = _packPsw(_regFlag);

        u16 af =  ((u16)_regA << 8) | psw;

        _stackPush(af);

        return;
    }
    
    u16 data = _getRegPairData(regPairCode);
    _stackPush(data);
}


void Cpu::_pop() { 
    u8 regPairCode = (_regCmd & 0b00110000) >> 4;

    u16 apsw = _stackPop();

    // Flags and A read
    if (regPairCode == 0b11) {
        _regFlag = _unpackPsw(apsw & 0xFF);

        _regA = apsw >> 8;

        return;
    }
    
    _setRegPairData(regPairCode, apsw);
}


void Cpu::_dad() { 
    u8 regPairCode = (_regCmd & 0b00110000) >> 4;

    u16 data = _getRegPairData(regPairCode);
    u16 hl   = _getRegPairData(0b10);

    u32 res  = data + hl;

    _regFlag.carry = ((res & 0xFFFF0000) != false) & 0b1;

    _setRegPairData(0b10, (u16)res);
}


void Cpu::_inx() { 
    u8 regPairCode = (_regCmd & 0b00110000) >> 4;

    u16 data = _getRegPairData(regPairCode);
    data++;

    _setRegPairData(regPairCode, data);
}


void Cpu::_dcx() { 
    u8 regPairCode = (_regCmd & 0b00110000) >> 4;

    u16 data = _getRegPairData(regPairCode);
    data--;

    _setRegPairData(regPairCode, data);
}


void Cpu::_xchg() {
    u8 h = _regH;
    u8 l = _regL;

    _regH = _regD;
    _regL = _regE;

    _regD = h;
    _regE = l;
}


void Cpu::_xthl() { 
    u8 h = _regH;
    u8 l = _regL;

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
    u8  lowAdr = _memoryRead();
    _regAdr = (_memoryRead() << 8) | lowAdr;

    _memoryWrite(_regA);
}


void Cpu::_lda() { 
    u8  lowAdr = _memoryRead();
    _regAdr = (_memoryRead() << 8) | lowAdr;

    _regA = _bus.memoryRead(_regAdr);
}


void Cpu::_shld() { 
    u8  lowAdr = _memoryRead();
    u16 adr    = ((u16)_memoryRead() << 8) | lowAdr;

    _bus.memoryWrite(adr,   _regL);
    _bus.memoryWrite(adr+1, _regH);
}


void Cpu::_lhld() { 
    u8  lowAdr = _memoryRead();
    u16 adr    = (_memoryRead() << 8) | lowAdr;

    _regL = _bus.memoryRead(adr);
    _regH = _bus.memoryRead(adr+1);
}

// Jump instructions
void Cpu::_pchl() { 
    _prgCounter = (_regH << 8) | _regL;
}

void Cpu::_jmp(u16 adr, bool cond) {
    if (!cond) return;

    _prgCounter = adr; 
}

void Cpu::_jmp(bool cond) {
    u8  lowAdr = _memoryRead();
    u16 adr    = (_memoryRead() << 8) | lowAdr;
    
    _jmp(adr, cond);
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
void Cpu::_call(u16 adr, bool cond) {
    if (!cond) return;

    _stackPush(_prgCounter);
    _prgCounter = adr;
}

void Cpu::_call(bool cond) { 
    u8  lowAdr = _memoryRead();
    u16 adr    = (_memoryRead() << 8) | lowAdr;
    
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

    u16 adr    = _stackPop();
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
    u8 rstCode = (_regCmd & 0b00111000) >> 3;

    _stackPush(_prgCounter);

    _prgCounter = rstCode << 3;
}

// Interrupt Flip-Flop instructions
void Cpu::_ei() { _interruptsEnabled = true; }
void Cpu::_di() { _interruptsEnabled = false; }

// IO instructions
void Cpu::_in()  { 
    u8 port = _memoryRead();
    _regA = _portRead(port);
}

void Cpu::_out() { 
    u8 port = _memoryRead();
    _portWrite(port, _regA);
}

// Hlt instruction
void Cpu::_hlt() { _hold = true; }