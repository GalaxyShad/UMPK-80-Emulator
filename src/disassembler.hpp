#pragma once 

#include <algorithm>
#include <cinttypes>
#include <string>

#include "bus.hpp"

std::string toUpper(const std::string src) {
    std::string result = src;

    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
}

class Disassembler {
    public:
        Disassembler(Bus& bus) : _bus(bus) {};

        std::string translate() {
            if (_prgCounter >= 0x1000) return "EOF"; 

            uint8_t opcode = _bus.memoryRead(_prgCounter);

            Instruction instruction = _instructions[opcode];

            instruction.name += " ";
            for (int i = 1; i < instruction.bytes; i++) {
                char value[8];
                sprintf(value, "%02x", _bus.memoryRead(_prgCounter + instruction.bytes-i));
                instruction.name += std::string(value);
            }

            _prgCounter += instruction.bytes;

            return toUpper(instruction.name);
        }

        void reset() { _prgCounter = 0; } 
        void reset(uint16_t prgCounter) { _prgCounter = prgCounter; } 

        uint8_t getInstructionBytes(uint8_t opcode) { return _instructions[opcode].bytes; }

        std::string getInstruction(uint8_t cmd) {
            return _instructions[cmd].name;
        }

        bool isEof() { return _prgCounter >= 0x1000; }
        
    private:
        Bus& _bus;

        uint16_t _prgCounter = 0x0000;

        struct Instruction {
            std::string     name;
            int             bytes;
        };

        const Instruction _instructions[256] = {
/*       // 0x00                0x01                    0x02                    0x03                    0x04                0x05                    0x06                0x07                0x08                0x09                0x0A                    0x0B                    0x0C                0x0D                    0x0E                0x0F            //     
/* 0x00 */  {"nop",     1},     {"lxi B,D16",   3},     {"stax B",      1},     {"inx B",       1},     {"inr B",   1},     {"dcr B",       1},     {"mvi B",   2},     {"rlc",     1},     {"UNKI",    1},     {"dad B",   1},     {"ldax B",      1},     {"dcx B",       1},     {"inr C",   1},     {"dcr C",       1},     {"mvi C",   2},     {"rrc",     1}, // 0x00
/* 0x01 */  {"UNKI",    1},     {"lxi D,D16",   3},     {"stax D",      1},     {"inx D",       1},     {"inr D",   1},     {"dcr D",       1},     {"mvi D",   2},     {"ral",     1},     {"UNKI",    1},     {"dad D",   1},     {"ldax D",      1},     {"dcx D",       1},     {"inr E",   1},     {"dcr E",       1},     {"mvi E",   2},     {"rar",     1}, // 0x10
/* 0x02 */  {"UNKI",    1},     {"lxi H,D16",   3},     {"shld ADR",    3},     {"inx H",       1},     {"inr H",   1},     {"dcr H",       1},     {"mvi H",   2},     {"daa",     1},     {"UNKI",    1},     {"dad H",   1},     {"lhld ADR",    3},     {"dcx H",       1},     {"inr L",   1},     {"dcr L",       1},     {"mvi L",   2},     {"cma",     1}, // 0x20
/* 0x03 */  {"UNKI",    1},     {"lxi SP,D16",  3},     {"sta ADR",     3},     {"inx SP",      1},     {"inr M",   1},     {"dcr M",       1},     {"mvi M",   2},     {"stc",     1},     {"UNKI",    1},     {"dad SP",  1},     {"lda ADR",     3},     {"dcx SP",      1},     {"inr A",   1},     {"dcr A",       1},     {"mvi A",   2},     {"cmc",     1}, // 0x30
/* 0x04 */  {"mov B,B", 1},     {"mov B,C",     1},     {"mov B,D",     1},     {"mov B,E",     1},     {"mov B,H", 1},     {"mov B,L",     1},     {"mov B,M", 1},     {"mov B,A", 1},     {"mov C,B", 1},     {"mov C,C", 1},     {"mov C,D",     1},     {"mov C,E",     1},     {"mov C,H", 1},     {"mov C,L",     1},     {"mov C,M", 1},     {"mov C,A", 1}, // 0x40
/* 0x05 */  {"mov D,B", 1},     {"mov D,C",     1},     {"mov D,D",     1},     {"mov D,E",     1},     {"mov D,H", 1},     {"mov D,L",     1},     {"mov D,M", 1},     {"mov D,A", 1},     {"mov E,B", 1},     {"mov E,C", 1},     {"mov E,D",     1},     {"mov E,E",     1},     {"mov E,H", 1},     {"mov E,L",     1},     {"mov E,M", 1},     {"mov E,A", 1}, // 0x50
/* 0x06 */  {"mov H,B", 1},     {"mov H,C",     1},     {"mov H,D",     1},     {"mov H,E",     1},     {"mov H,H", 1},     {"mov H,L",     1},     {"mov H,M", 1},     {"mov H,A", 1},     {"mov L,B", 1},     {"mov L,C", 1},     {"mov L,D",     1},     {"mov L,E",     1},     {"mov L,H", 1},     {"mov L,L",     1},     {"mov L,M", 1},     {"mov L,A", 1}, // 0x60
/* 0x07 */  {"mov M,B", 1},     {"mov M,C",     1},     {"mov M,D",     1},     {"mov M,E",     1},     {"mov M,H", 1},     {"mov M,L",     1},     {"hlt",     1},     {"mov M,A", 1},     {"mov A,B", 1},     {"mov A,C", 1},     {"mov A,D",     1},     {"mov A,E",     1},     {"mov A,H", 1},     {"mov A,L",     1},     {"mov A,M", 1},     {"mov A,A", 1}, // 0x70
/* 0x08 */  {"add B",   1},     {"add C",       1},     {"add D",       1},     {"add E",       1},     {"add H",   1},     {"add L",       1},     {"add M",   1},     {"add A",   1},     {"adc B",   1},     {"adc C",   1},     {"adc D",       1},     {"adc E",       1},     {"adc H",   1},     {"adc L",       1},     {"adc M",   1},     {"adc A",   1}, // 0x80
/* 0x09 */  {"sub B",   1},     {"sub C",       1},     {"sub D",       1},     {"sub E",       1},     {"sub H",   1},     {"sub L",       1},     {"sub M",   1},     {"sub A",   1},     {"sbb B",   1},     {"sbb C",   1},     {"sbb D",       1},     {"sbb E",       1},     {"sbb H",   1},     {"sbb L",       1},     {"sbb M",   1},     {"sbb A",   1}, // 0x90
/* 0x0A */  {"ana B",   1},     {"ana C",       1},     {"ana D",       1},     {"ana E",       1},     {"ana H",   1},     {"ana L",       1},     {"ana M",   1},     {"ana A",   1},     {"xra B",   1},     {"xra C",   1},     {"xra D",       1},     {"xra E",       1},     {"xra H",   1},     {"xra L",       1},     {"xra M",   1},     {"xra A",   1}, // 0xA0
/* 0x0B */  {"ora B",   1},     {"ora C",       1},     {"ora D",       1},     {"ora E",       1},     {"ora H",   1},     {"ora L",       1},     {"ora M",   1},     {"ora A",   1},     {"cmp B",   1},     {"cmp C",   1},     {"cmp D",       1},     {"cmp E",       1},     {"cmp H",   1},     {"cmp L",       1},     {"cmp M",   1},     {"cmp A",   1}, // 0xB0
/* 0x0C */  {"rnz",     1},     {"pop B",       1},     {"jnz ADR",     3},     {"jmp ADR",     3},     {"cnz ADR", 3},     {"push B",      1},     {"adi D8",  2},     {"rst 0",   1},     {"rz",      1},     {"ret",     1},     {"jz ADR",      3},     {"UNKI",        1},     {"cz ADR",  3},     {"call ADR",    3},     {"aci D8",  2},     {"rst 1",   1}, // 0xC0
/* 0x0D */  {"rnc",     1},     {"pop D",       1},     {"jnc ADR",     3},     {"out PORT",    2},     {"cnc ADR", 3},     {"push D",      1},     {"sui D8",  2},     {"rst 2",   1},     {"rc",      1},     {"UNKI",    1},     {"jc ADR",      3},     {"in PORT",     2},     {"cc ADR",  3},     {"UNKI",        1},     {"sbi D8",  2},     {"rst 3",   1}, // 0xD0
/* 0x0E */  {"rpo",     1},     {"pop H",       1},     {"jpo ADR",     3},     {"xthl",        1},     {"cpo ADR", 3},     {"push H",      1},     {"ani D8",  2},     {"rst 4",   1},     {"rpe",     1},     {"pchl",    1},     {"jpe ADR",     3},     {"xchg",        1},     {"cpe ADR", 3},     {"UNKI",        1},     {"xri D8",  2},     {"rst 5",   1}, // 0xE0
/* 0x0F */  {"rp",      1},     {"pop PSW",     1},     {"jp ADR",      3},     {"di",          1},     {"cp ADR",  3},     {"push PSW",    1},     {"ori D8",  2},     {"rst 6",   1},     {"rm",      1},     {"sphl",    1},     {"jm ADR",      3},     {"ei",          1},     {"cm ADR",  3},     {"UNKI",        1},     {"cpi D8",  2},     {"rst 7",   1}, // 0xF0
        //  0x00                0x01                    0x02                    0x03                    0x04                0x05                    0x06                0x07                0x08                0x09                0x0A                    0x0B                    0x0C                0x0D                    0x0E                0x0F            //   
        };
};