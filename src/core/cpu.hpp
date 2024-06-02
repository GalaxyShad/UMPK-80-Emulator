#pragma once

#include "bus.hpp"

struct CpuFlagsMapping { 
    u8 sign: 1,
            zero: 1, 
            :1, 
            auxcarry: 1, 
            :1,  
            parity: 1, 
            :1, 
            carry: 1; 
};

class Cpu {
public:
    Cpu(Bus& bus);

    void    tick();
    void    reset();
    bool    isHold() const { return _hold; };

    u8  getCommandRegister() const   { return _regCmd;       }
    u16 getAdressRegister()  const   { return _regAdr;       }

    u16 getStackPointer() const         { return _stackPointer; }
    void     setStackPointer(u16 sp)    { _stackPointer = sp;   }
    
    u16 getProgramCounter() const       { return _prgCounter; }
    void     setProgramCounter(u16 adr) { _prgCounter = adr;  };

    u8         getRegisterFlags() const        { return _packPsw(_regFlag); }
    void            setRegisterFlags(u8 data)  { _regFlag = _unpackPsw(data); }

    CpuFlagsMapping getFlags() const                { return _regFlag;  }
    void            setFlags(CpuFlagsMapping flags) { _regFlag = flags; }

    enum class Register {
        B, C,
        D, E,
        H, L,
        M, A
    };

    u8 A() { return getRegister(Register::A); }
    u8 B() { return getRegister(Register::B); }
    u8 C() { return getRegister(Register::C); }
    u8 D() { return getRegister(Register::D); }
    u8 E() { return getRegister(Register::E); }
    u8 H() { return getRegister(Register::H); }
    u8 L() { return getRegister(Register::L); }

    u8 getRegister(Register reg) const         { return _getRegData((u8)reg); }
    void    setRegister(Register reg, u8 data) { return _setRegData((u8)reg, data); }

    void interruptRst(int rstNum) {
        if (rstNum < 8 && rstNum >= 0) _call(rstNum * 8, true);
    }

    void forceCall(u16 adr) { _call(adr); }
    void forceJump(u16 adr) { _jmp(adr); }

private:
    Bus&        _bus;

    bool        _enableInterrupts  = false;

    bool        _hold              = false;
    bool        _interruptsEnabled = false;

    u8     _regCmd         = 0x00;
    u16    _regAdr         = 0x0000;
    u16    _prgCounter     = 0x0000;
    u16    _stackPointer   = 0xFFFF;

    u8     _regA = 0x00, _regT = 0x00;
    u8     _regB = 0x00, _regC = 0x00;
    u8     _regD = 0x00, _regE = 0x00;
    u8     _regH = 0x00, _regL = 0x00;

    u8*    _registers[8] = {
        &_regB, &_regC, 
        &_regD, &_regE, 
        &_regH, &_regL, 
        nullptr,&_regA,
    };

    CpuFlagsMapping _regFlag;
    
    typedef void (Cpu::*instructionFunction_t)(void); 
    const instructionFunction_t _instructions[256] = {
        //  0x00         0x01         0x02         0x03         0x04         0x05         0x06         0x07         0x08         0x09         0x0A         0x0B         0x0C         0x0D         0x0E         0x0F        //     
/* 0x00 */  &Cpu::_nop,  &Cpu::_lxi,  &Cpu::_stax, &Cpu::_inx,  &Cpu::_inr,  &Cpu::_dcr,  &Cpu::_mvi,  &Cpu::_rlc,  &Cpu::_nop,  &Cpu::_dad,  &Cpu::_ldax, &Cpu::_dcx,  &Cpu::_inr,  &Cpu::_dcr,  &Cpu::_mvi,  &Cpu::_rrc, // 0x00
/* 0x10 */  &Cpu::_nop,  &Cpu::_lxi,  &Cpu::_stax, &Cpu::_inx,  &Cpu::_inr,  &Cpu::_dcr,  &Cpu::_mvi,  &Cpu::_ral,  &Cpu::_nop,  &Cpu::_dad,  &Cpu::_ldax, &Cpu::_dcx,  &Cpu::_inr,  &Cpu::_dcr,  &Cpu::_mvi,  &Cpu::_rar, // 0x10
/* 0x20 */  &Cpu::_nop,  &Cpu::_lxi,  &Cpu::_shld, &Cpu::_inx,  &Cpu::_inr,  &Cpu::_dcr,  &Cpu::_mvi,  &Cpu::_daa,  &Cpu::_nop,  &Cpu::_dad,  &Cpu::_lhld, &Cpu::_dcx,  &Cpu::_inr,  &Cpu::_dcr,  &Cpu::_mvi,  &Cpu::_cma, // 0x20
/* 0x30 */  &Cpu::_nop,  &Cpu::_lxi,  &Cpu::_sta,  &Cpu::_inx,  &Cpu::_inr,  &Cpu::_dcr,  &Cpu::_mvi,  &Cpu::_stc,  &Cpu::_nop,  &Cpu::_dad,  &Cpu::_lda,  &Cpu::_dcx,  &Cpu::_inr,  &Cpu::_dcr,  &Cpu::_mvi,  &Cpu::_cmc, // 0x30
/* 0x40 */  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov, // 0x40
/* 0x50 */  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov, // 0x50
/* 0x60 */  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov, // 0x60
/* 0x70 */  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_hlt,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov,  &Cpu::_mov, // 0x70
/* 0x80 */  &Cpu::_add,  &Cpu::_add,  &Cpu::_add,  &Cpu::_add,  &Cpu::_add,  &Cpu::_add,  &Cpu::_add,  &Cpu::_add,  &Cpu::_adc,  &Cpu::_adc,  &Cpu::_adc,  &Cpu::_adc,  &Cpu::_adc,  &Cpu::_adc,  &Cpu::_adc,  &Cpu::_adc, // 0x80
/* 0x90 */  &Cpu::_sub,  &Cpu::_sub,  &Cpu::_sub,  &Cpu::_sub,  &Cpu::_sub,  &Cpu::_sub,  &Cpu::_sub,  &Cpu::_sub,  &Cpu::_sbb,  &Cpu::_sbb,  &Cpu::_sbb,  &Cpu::_sbb,  &Cpu::_sbb,  &Cpu::_sbb,  &Cpu::_sbb,  &Cpu::_sbb, // 0x90
/* 0xA0 */  &Cpu::_ana,  &Cpu::_ana,  &Cpu::_ana,  &Cpu::_ana,  &Cpu::_ana,  &Cpu::_ana,  &Cpu::_ana,  &Cpu::_ana,  &Cpu::_xra,  &Cpu::_xra,  &Cpu::_xra,  &Cpu::_xra,  &Cpu::_xra,  &Cpu::_xra,  &Cpu::_xra,  &Cpu::_xra, // 0xA0
/* 0xB0 */  &Cpu::_ora,  &Cpu::_ora,  &Cpu::_ora,  &Cpu::_ora,  &Cpu::_ora,  &Cpu::_ora,  &Cpu::_ora,  &Cpu::_ora,  &Cpu::_cmp,  &Cpu::_cmp,  &Cpu::_cmp,  &Cpu::_cmp,  &Cpu::_cmp,  &Cpu::_cmp,  &Cpu::_cmp,  &Cpu::_cmp, // 0xB0
/* 0xC0 */  &Cpu::_rnz,  &Cpu::_pop,  &Cpu::_jnz,  &Cpu::_jmp,  &Cpu::_cnz,  &Cpu::_push, &Cpu::_adi,  &Cpu::_rst,  &Cpu::_rz,   &Cpu::_ret,  &Cpu::_jz,   &Cpu::_jmp,  &Cpu::_cz,   &Cpu::_call, &Cpu::_aci,  &Cpu::_rst, // 0xC0
/* 0xD0 */  &Cpu::_rnc,  &Cpu::_pop,  &Cpu::_jnc,  &Cpu::_out,  &Cpu::_cnc,  &Cpu::_push, &Cpu::_sui,  &Cpu::_rst,  &Cpu::_rc,   &Cpu::_ret,  &Cpu::_jc,   &Cpu::_in,   &Cpu::_cc,   &Cpu::_call, &Cpu::_sbi,  &Cpu::_rst, // 0xD0
/* 0xE0 */  &Cpu::_rpo,  &Cpu::_pop,  &Cpu::_jpo,  &Cpu::_xthl, &Cpu::_cpo,  &Cpu::_push, &Cpu::_ani,  &Cpu::_rst,  &Cpu::_rpe,  &Cpu::_pchl, &Cpu::_jpe,  &Cpu::_xchg, &Cpu::_cpe,  &Cpu::_call, &Cpu::_xri,  &Cpu::_rst, // 0xE0
/* 0xF0 */  &Cpu::_rp,   &Cpu::_pop,  &Cpu::_jp,   &Cpu::_di,   &Cpu::_cp,   &Cpu::_push, &Cpu::_ori,  &Cpu::_rst,  &Cpu::_rm,   &Cpu::_sphl, &Cpu::_jm,   &Cpu::_ei,   &Cpu::_cm,   &Cpu::_call, &Cpu::_cpi,  &Cpu::_rst, // 0xF0
        //  0x00         0x01         0x02         0x03         0x04         0x05         0x06         0x07         0x08         0x09         0x0A         0x0B         0x0C         0x0D         0x0E         0x0F        // 
    };

    // Machine cycles
    void        _readCommand();
    void        _readCommand(u8 opcode);

    void        _memoryWrite(u8 data);
    u8     _memoryRead();

    void        _stackPush(u16 data);
    u16    _stackPop();

    void        _portWrite(u8 port, u8 data);
    u8     _portRead(u8 port);

    u8         _packPsw(CpuFlagsMapping flags) const;
    CpuFlagsMapping _unpackPsw(u8 psw) const;

    // Register operations
    u8     _getRegData(u8 regCode) const;
    void        _setRegData(u8 regCode, u8 data);

    u16    _getRegPairData(u8 regPairCode);
    void        _setRegPairData(u8 regPairCode, u16 data);
    void        _setRegPairData(u8 regPairCode, u8 dataA, u8 dataB);

    // Utility
    void        _updateFlagsState(u16 result);

    // Nop instruction
    void _nop();

    // Carry bit instructions
    void _stc();
    void _cmc();

    // Single register instructions
    void _inr();
    void _dcr();
    void _cma();
    void _daa();

    // Data transfer instructions
    void _mov();
    void _stax();
    void _ldax();

    // Arithmetical or logical instructions
    void _add();
    void _adc();
    void _sub();
    void _sbb();
    void _ana();
    void _xra();
    void _ora();
    void _cmp();

    // Immediate instructions
    void _lxi();
    void _mvi();
    void _adi();
    void _aci();
    void _sui();
    void _sbi();
    void _ani();
    void _xri();
    void _ori();
    void _cpi();

    // Rotate accumulator instructions
    void _rlc();
    void _rrc();
    void _ral();
    void _rar();

    // Register pair instructions
    void _push();
    void _pop();
    void _dad();
    void _inx();
    void _dcx();

    void _xchg();
    void _xthl();
    void _sphl();

    // Direct adressing instructions
    void _sta();
    void _lda();
    void _shld();
    void _lhld();

    // Jump instructions
    void _pchl();
    void _jmp(u16 adr, bool cond = true);
    void _jmp(bool cond = true);
    void _jmp();
    void _jc();
    void _jnc();
    void _jz();
    void _jnz();
    void _jp();
    void _jm();
    void _jpe();
    void _jpo();

    // Call instructions
    void _call(u16 adr, bool cond = true);
    void _call(bool cond = true);
    void _call();
    void _cc();
    void _cnc();
    void _cz();
    void _cnz();
    void _cp();
    void _cm();
    void _cpe();
    void _cpo();

    // Return instructions 
    void _ret(bool cond = true);
    void _ret();
    void _rc();
    void _rnc();
    void _rz();
    void _rnz();
    void _rm();
    void _rp();
    void _rpe();
    void _rpo();

    // Rst instruction
    void _rst();

    // Interrupt Flip-Flop instructions
    void _ei();
    void _di();

    // IO instructions
    void _in();
    void _out();

    // Hlt instruction
    void _hlt();
};