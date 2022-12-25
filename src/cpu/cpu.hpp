#pragma once

#include <cinttypes>

#include "./bus/adress_bus.hpp"
#include "./bus/data_bus.hpp"

class Cpu {
    public:
        void    tick();

    private:
        AdressBus&  _adrBus;
        DataBus&    _dataBus;

        uint8_t     _regCmd;
        uint16_t    _regAdr;
        uint16_t    _prgCounter;
        uint16_t    _stackPointer;

        uint8_t     _regA;
        uint8_t     _regB, _regC;
        uint8_t     _regD, _regE;
        uint8_t     _regH, _regL;

        uint8_t*    _registers[8] = {
            &_regB, &_regC, 
            &_regD, &_regE, 
            &_regH, &_regL, 
            NULL,   &_regA, 
        };

        struct _regFlag { uint8_t sign: 1, zero: 1, auxcarry: 1, parry: 1, carry: 1; };

        void        _readCommand();

        uint8_t     _getRegData(uint8_t regCode);
        void        _setRegData(uint8_t regCode, uint8_t data);

        void        _memoryWrite(uint8_t data);
        uint8_t     _memoryRead();

        void        _portWrite(uint8_t port, uint8_t data);
        uint8_t     _portRead(uint8_t port);

        void        _stackPush(uint16_t data);
        uint16_t    _stackPop();

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