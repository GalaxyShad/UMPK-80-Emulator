#pragma once

#include <cinttypes>

#include "bus.hpp"


struct CpuFlagsMapping { 
    uint8_t sign: 1, zero: 1, :1, auxcarry: 1, :1,  parry: 1, :1, carry: 1; 
};

class Cpu {
    public:
        Cpu(Bus& bus);

        void    tick();
        void    reset();
        bool    isHold() { return _hold; };
        void    setProgramCounter(uint16_t adr) { _prgCounter = adr; };

        uint8_t  getCommandRegister()   { return _regCmd;       }
        uint16_t getAdressRegister()    { return _regAdr;       }
        uint16_t getStackPointer()      { return _stackPointer; }
        uint16_t getProgramCounter()    { return _prgCounter;   }
        CpuFlagsMapping getFlags()      { return _regFlag;      }

        enum Register {
            B, C,
            D, E,
            H, L,
            M, A
        };

        uint8_t getRegister(Register reg) { return _getRegData((uint8_t)reg); }

    private:
        Bus&  _bus;

        bool        _enableInterrupts  = false;

        bool        _hold              = false;
        bool        _interruptsEnabled = false;

        uint8_t     _regCmd         = 0x00;
        uint16_t    _regAdr         = 0x0000;
        uint16_t    _prgCounter     = 0x0000;
        uint16_t    _stackPointer   = 0xFFFF;

        uint8_t     _regA = 0x00, _regT = 0x00;
        uint8_t     _regB = 0x00, _regC = 0x00;
        uint8_t     _regD = 0x00, _regE = 0x00;
        uint8_t     _regH = 0x00, _regL = 0x00;

        uint8_t*    _registers[8] = {
            &_regB, &_regC, 
            &_regD, &_regE, 
            &_regH, &_regL, 
            NULL,   &_regA, 
        };

        CpuFlagsMapping _regFlag;
        
        typedef void (Cpu::*instructionFunction_t)(void); 
        #define UNKI    nullptr
        const instructionFunction_t _instructions[256] = {
        //  0x00   0x01   0x02   0x03   0x04   0x05   0x06   0x07   0x08   0x09   0x0A   0x0B   0x0C   0x0D   0x0E   0x0F  //     
            _nop,  _lxi,  _stax, _inx,  _inr,  _dcr,  _mvi,  _rlc,  UNKI,  _dad,  _ldax, _dcx,  _inr,  _dcr,  _mvi,  _rrc, // 0x00
            UNKI,  _lxi,  _stax, _inx,  _inr,  _dcr,  _mvi,  _ral,  UNKI,  _dad,  _ldax, _dcx,  _inr,  _dcr,  _mvi,  _rar, // 0x10
            UNKI,  _lxi,  _shld, _inx,  _inr,  _dcr,  _mvi,  _daa,  UNKI,  _dad,  _lhld, _dcx,  _inr,  _dcr,  _mvi,  _cma, // 0x20
            UNKI,  _lxi,  _sta,  _inx,  _inr,  _dcr,  _mvi,  _stc,  UNKI,  _dad,  _lda,  _dcx,  _inr,  _dcr,  _mvi,  _cmc, // 0x30
            _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov, // 0x40
            _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov, // 0x50
            _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov, // 0x60
            _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _hlt,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov,  _mov, // 0x70
            _add,  _add,  _add,  _add,  _add,  _add,  _add,  _add,  _adc,  _adc,  _adc,  _adc,  _adc,  _adc,  _adc,  _adc, // 0x80
            _sub,  _sub,  _sub,  _sub,  _sub,  _sub,  _sub,  _sub,  _sbb,  _sbb,  _sbb,  _sbb,  _sbb,  _sbb,  _sbb,  _sbb, // 0x90
            _ana,  _ana,  _ana,  _ana,  _ana,  _ana,  _ana,  _ana,  _xra,  _xra,  _xra,  _xra,  _xra,  _xra,  _xra,  _xra, // 0xA0
            _ora,  _ora,  _ora,  _ora,  _ora,  _ora,  _ora,  _ora,  _cmp,  _cmp,  _cmp,  _cmp,  _cmp,  _cmp,  _cmp,  _cmp, // 0xB0
            _rnz,  _pop,  _jnz,  _jmp,  _cnz,  _push, _adi,  _rst,  _rz,   _ret,  _jz,   UNKI,  _cz,   _call, _aci,  _rst, // 0xC0
            _rnc,  _pop,  _jnc,  _out,  _cnc,  _push, _sui,  _rst,  _rc,   UNKI,  _jc,   _in,   _cc,   UNKI,  _sbi,  _rst, // 0xD0
            _rpo,  _pop,  _jpo,  _xthl, _cpo,  _push, _ani,  _rst,  _rpe,  _pchl, _jpe,  _xchg, _cpe,  UNKI,  _xri,  _rst, // 0xE0
            _rp,   _pop,  _jp,   _di,   _cp,   _push, _ori,  _rst,  _rm,   _sphl, _jm,   _ei,   _cm,   UNKI,  _cpi,  _rst, // 0xF0
        //  0x00   0x01   0x02   0x03   0x04   0x05   0x06   0x07   0x08   0x09   0x0A   0x0B   0x0C   0x0D   0x0E   0x0F  // 
        };
        #undef UNKI


        // Machine cycles
        void        _readCommand();

        void        _memoryWrite(uint8_t data);
        uint8_t     _memoryRead();

        void        _stackPush(uint16_t data);
        uint16_t    _stackPop();

        void        _portWrite(uint8_t port, uint8_t data);
        uint8_t     _portRead(uint8_t port);


        // Register operations
        uint8_t     _getRegData(uint8_t regCode);
        void        _setRegData(uint8_t regCode, uint8_t data);

        uint16_t    _getRegPairData(uint8_t regPairCode);
        void        _setRegPairData(uint8_t regPairCode, uint16_t data);
        void        _setRegPairData(uint8_t regPairCode, uint8_t dataA, uint8_t dataB);

        // Utility
        void        _updateFlagsState(uint16_t result);

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