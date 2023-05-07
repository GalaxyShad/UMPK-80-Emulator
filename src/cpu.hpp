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

        uint8_t  getCurCmdTimePeriod()  { return _numbersOfTimePeriods[_regCmd].main; }
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

        uint8_t getRegister(Register reg)               { return _getRegData((uint8_t)reg); }
        void    setRegister(Register reg, uint8_t data) { return _setRegData(reg, data); }

        void interruptRst(int rstNum) {
            if (rstNum < 8 && rstNum >= 0) _call(rstNum * 8, true);
        }

        void interruptCall(uint16_t adr) {
            if (_enableInterrupts) _call(adr);
        }

    private:
        Bus&        _bus;

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

        struct CpuTimePeriod {
            char str[20];
            uint8_t main;
            uint8_t ifcond = 0;
        };


        const CpuTimePeriod _numbersOfTimePeriods[256] = {
/*       // 0x00                0x01                    0x02                    0x03                    0x04                     0x05                    0x06                0x07                0x08                0x09                0x0A                    0x0B                    0x0C                     0x0D                    0x0E                0x0F            //     
/* 0x00 */  {"nop",     4},     {"lxi B,D16",  10},     {"stax B",      7},     {"inx B",       5},     {"inr B",   5},          {"dcr B",       5},     {"mvi B",   7},     {"rlc",     4},     {"UNKI",    1},     {"dad B",  10},     {"ldax B",      7},     {"dcx B",       5},     {"inr C",   5},          {"dcr C",       5},     {"mvi C",   7},     {"rrc",     4}, // 0x00
/* 0x01 */  {"UNKI",    4},     {"lxi D,D16",  10},     {"stax D",      7},     {"inx D",       5},     {"inr D",   5},          {"dcr D",       5},     {"mvi D",   7},     {"ral",     4},     {"UNKI",    1},     {"dad D",  10},     {"ldax D",      7},     {"dcx D",       5},     {"inr E",   5},          {"dcr E",       5},     {"mvi E",   7},     {"rar",     4}, // 0x10
/* 0x02 */  {"UNKI",    4},     {"lxi H,D16",  10},     {"shld ADR",   16},     {"inx H",       5},     {"inr H",   5},          {"dcr H",       5},     {"mvi H",   7},     {"daa",     4},     {"UNKI",    1},     {"dad H",  10},     {"lhld ADR",   16},     {"dcx H",       5},     {"inr L",   5},          {"dcr L",       5},     {"mvi L",   7},     {"cma",     4}, // 0x20
/* 0x03 */  {"UNKI",    4},     {"lxi SP,D16", 10},     {"sta ADR",    13},     {"inx SP",      5},     {"inr M",  10},          {"dcr M",      10},     {"mvi M",  10},     {"stc",     4},     {"UNKI",    1},     {"dad SP", 10},     {"lda ADR",    13},     {"dcx SP",      5},     {"inr A",   5},          {"dcr A",       5},     {"mvi A",   7},     {"cmc",     4}, // 0x30
/* 0x04 */  {"mov B,B", 5},     {"mov B,C",     5},     {"mov B,D",     5},     {"mov B,E",     5},     {"mov B,H", 5},          {"mov B,L",     5},     {"mov B,M", 7},     {"mov B,A", 5},     {"mov C,B", 5},     {"mov C,C", 5},     {"mov C,D",     5},     {"mov C,E",     5},     {"mov C,H", 5},          {"mov C,L",     5},     {"mov C,M", 7},     {"mov C,A", 5}, // 0x40
/* 0x05 */  {"mov D,B", 5},     {"mov D,C",     5},     {"mov D,D",     5},     {"mov D,E",     5},     {"mov D,H", 5},          {"mov D,L",     5},     {"mov D,M", 7},     {"mov D,A", 5},     {"mov E,B", 5},     {"mov E,C", 5},     {"mov E,D",     5},     {"mov E,E",     5},     {"mov E,H", 5},          {"mov E,L",     5},     {"mov E,M", 7},     {"mov E,A", 5}, // 0x50
/* 0x06 */  {"mov H,B", 5},     {"mov H,C",     5},     {"mov H,D",     5},     {"mov H,E",     5},     {"mov H,H", 5},          {"mov H,L",     5},     {"mov H,M", 7},     {"mov H,A", 5},     {"mov L,B", 5},     {"mov L,C", 5},     {"mov L,D",     5},     {"mov L,E",     5},     {"mov L,H", 5},          {"mov L,L",     5},     {"mov L,M", 7},     {"mov L,A", 5}, // 0x60
/* 0x07 */  {"mov M,B", 7},     {"mov M,C",     7},     {"mov M,D",     7},     {"mov M,E",     7},     {"mov M,H", 7},          {"mov M,L",     7},     {"hlt",     7},     {"mov M,A", 7},     {"mov A,B", 5},     {"mov A,C", 5},     {"mov A,D",     5},     {"mov A,E",     5},     {"mov A,H", 5},          {"mov A,L",     5},     {"mov A,M", 7},     {"mov A,A", 5}, // 0x70
/* 0x08 */  {"add B",   4},     {"add C",       4},     {"add D",       4},     {"add E",       4},     {"add H",   4},          {"add L",       4},     {"add M",   7},     {"add A",   4},     {"adc B",   4},     {"adc C",   4},     {"adc D",       4},     {"adc E",       4},     {"adc H",   4},          {"adc L",       4},     {"adc M",   7},     {"adc A",   4}, // 0x80
/* 0x09 */  {"sub B",   4},     {"sub C",       4},     {"sub D",       4},     {"sub E",       4},     {"sub H",   4},          {"sub L",       4},     {"sub M",   7},     {"sub A",   4},     {"sbb B",   4},     {"sbb C",   4},     {"sbb D",       4},     {"sbb E",       4},     {"sbb H",   4},          {"sbb L",       4},     {"sbb M",   7},     {"sbb A",   4}, // 0x90
/* 0x0A */  {"ana B",   4},     {"ana C",       4},     {"ana D",       4},     {"ana E",       4},     {"ana H",   4},          {"ana L",       4},     {"ana M",   7},     {"ana A",   4},     {"xra B",   4},     {"xra C",   4},     {"xra D",       4},     {"xra E",       4},     {"xra H",   4},          {"xra L",       4},     {"xra M",   7},     {"xra A",   4}, // 0xA0
/* 0x0B */  {"ora B",   4},     {"ora C",       4},     {"ora D",       4},     {"ora E",       4},     {"ora H",   4},          {"ora L",       4},     {"ora M",   7},     {"ora A",   4},     {"cmp B",   4},     {"cmp C",   4},     {"cmp D",       4},     {"cmp E",       4},     {"cmp H",   4},          {"cmp L",       4},     {"cmp M",   7},     {"cmp A",   4}, // 0xB0
/* 0x0C */  {"rnz", 5, 11},     {"pop B",      10},     {"jnz ADR",    10},     {"jmp ADR",    10},     {"cnz ADR", 11, 17},     {"push B",      11},    {"adi D8",  7},     {"rst 0",  11},     {"rz",  5, 11},     {"ret",    10},     {"jz ADR",     10},     {"UNKI",        1},     {"cz ADR",  11, 17},     {"call ADR",   17},     {"aci D8",  7},     {"rst 1",  11}, // 0xC0
/* 0x0D */  {"rnc", 5, 11},     {"pop D",      10},     {"jnc ADR",    10},     {"out PORT",   10},     {"cnc ADR", 11, 17},     {"push D",      11},    {"sui D8",  7},     {"rst 2",  11},     {"rc",  5, 11},     {"UNKI",    1},     {"jc ADR",     10},     {"in PORT",    10},     {"cc ADR",  11, 17},     {"UNKI",        1},     {"sbi D8",  7},     {"rst 3",  11}, // 0xD0
/* 0x0E */  {"rpo", 5, 11},     {"pop H",      10},     {"jpo ADR",    10},     {"xthl",       18},     {"cpo ADR", 11, 17},     {"push H",      11},    {"ani D8",  7},     {"rst 4",  11},     {"rpe", 5, 11},     {"pchl",    5},     {"jpe ADR",    10},     {"xchg",        4},     {"cpe ADR", 11, 17},     {"UNKI",        1},     {"xri D8",  7},     {"rst 5",  11}, // 0xE0
/* 0x0F */  {"rp",  5, 11},     {"pop PSW",    10},     {"jp ADR",     10},     {"di",          4},     {"cp ADR",  11, 17},     {"push PSW",    11},    {"ori D8",  7},     {"rst 6",  11},     {"rm",  5, 11},     {"sphl",    5},     {"jm ADR",     10},     {"ei",          4},     {"cm ADR",  11, 17},     {"UNKI",        1},     {"cpi D8",  7},     {"rst 7",  11}, // 0xF0
        //  0x00                0x01                    0x02                    0x03                    0x04                     0x05                    0x06                0x07                0x08                0x09                0x0A                    0x0B                    0x0C                     0x0D                    0x0E                0x0F            //   
        };


        // Machine cycles
        void        _readCommand();
        void        _readCommand(uint8_t opcode);

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
        void _call(uint16_t adr, bool cond = true);
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