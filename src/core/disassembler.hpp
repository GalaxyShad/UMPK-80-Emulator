#pragma once

#include "inttypes.hpp"

class Disassembler {
public:
    struct Instruction {
        const char* mnemonic;
        int length;
        const char* operand;
    };

    struct DisassembleResult {
        u16 address;
        u8 bytesCount;
        u8 bytes[3];
        const Instruction* instruction;
        bool eof;
    };

    static const Instruction& getInstruction(u8 opcode) {
        // clang-format off
        static const Instruction instructions[256] = {
            // 0x00                 0x01                   0x02                   0x03                        0x04                       0x05                    0x06                   0x07                0x08                    0x09                    0x0A                    0x0B                        0x0C                       0x0D                            0x0E                       0x0F            //
/* 0X00 */  {"NOP",         1, ""}, {"LXI B",  3, "D16"}, {"STAX B",  1, ""},    {"INX B",   1, ""},         {"INR B",   1, ""},        {"DCR B",       1, ""}, {"MVI B",   2, "D8"},    {"RLC",     1, ""}, {"(UNDOC) NOP", 1, ""}, {"DAD B",       1, ""}, {"LDAX B",  1, ""},     {"DCX B",       1, ""},     {"INR C",   1, ""},        {"DCR C",        1, ""},        {"MVI C",   2, ""},       {"RRC",     1, ""}, // 0X00
/* 0X10 */  {"(UNDOC) NOP", 1, ""}, {"LXI D",  3, "D16"}, {"STAX D",  1, ""},    {"INX D",   1, ""},         {"INR D",   1, ""},        {"DCR D",       1, ""}, {"MVI D",   2, "D8"},    {"RAL",     1, ""}, {"(UNDOC) NOP", 1, ""}, {"DAD D",       1, ""}, {"LDAX D",  1, ""},     {"DCX D",       1, ""},     {"INR E",   1, ""},        {"DCR E",        1, ""},        {"MVI E",   2, ""},       {"RAR",     1, ""}, // 0X10
/* 0X20 */  {"(UNDOC) NOP", 1, ""}, {"LXI H",  3, "D16"}, {"SHLD",    3, "ADR"}, {"INX H",   1, ""},         {"INR H",   1, ""},        {"DCR H",       1, ""}, {"MVI H",   2, "D8"},    {"DAA",     1, ""}, {"(UNDOC) NOP", 1, ""}, {"DAD H",       1, ""}, {"LHLD",    3, "ADR"},  {"DCX H",       1, ""},     {"INR L",   1, ""},        {"DCR L",        1, ""},        {"MVI L",   2, ""},       {"CMA",     1, ""}, // 0X20
/* 0X30 */  {"(UNDOC) NOP", 1, ""}, {"LXI SP", 3, ""},    {"STA",     3, "ADR"}, {"INX SP",  1, ""},         {"INR M",   1, ""},        {"DCR M",       1, ""}, {"MVI M",   2, "D8"},    {"STC",     1, ""}, {"(UNDOC) NOP", 1, ""}, {"DAD SP",      1, ""}, {"LDA",     3, "ADR"},  {"DCX SP",      1, ""},     {"INR A",   1, ""},        {"DCR A",        1, ""},        {"MVI A",   2, ""},       {"CMC",     1, ""}, // 0X30
/* 0X40 */  {"MOV B,B",     1, ""}, {"MOV B,C", 1, ""},    {"MOV B,D", 1, ""},    {"MOV B,E", 1, ""},         {"MOV B,H", 1, ""},        {"MOV B,L",     1, ""}, {"MOV B,M", 1, ""},    {"MOV B,A", 1, ""}, {"MOV C,B",     1, ""}, {"MOV C,C",     1, ""}, {"MOV C,D", 1, ""},     {"MOV C,E",     1, ""},     {"MOV C,H", 1, ""},        {"MOV C,L",      1, ""},        {"MOV C,M", 1, ""},       {"MOV C,A", 1, ""}, // 0X40
/* 0X50 */  {"MOV D,B",     1, ""}, {"MOV D,C", 1, ""},    {"MOV D,D", 1, ""},    {"MOV D,E", 1, ""},         {"MOV D,H", 1, ""},        {"MOV D,L",     1, ""}, {"MOV D,M", 1, ""},    {"MOV D,A", 1, ""}, {"MOV E,B",     1, ""}, {"MOV E,C",     1, ""}, {"MOV E,D", 1, ""},     {"MOV E,E",     1, ""},     {"MOV E,H", 1, ""},        {"MOV E,L",      1, ""},        {"MOV E,M", 1, ""},       {"MOV E,A", 1, ""}, // 0X50
/* 0X60 */  {"MOV H,B",     1, ""}, {"MOV H,C", 1, ""},    {"MOV H,D", 1, ""},    {"MOV H,E", 1, ""},         {"MOV H,H", 1, ""},        {"MOV H,L",     1, ""}, {"MOV H,M", 1, ""},    {"MOV H,A", 1, ""}, {"MOV L,B",     1, ""}, {"MOV L,C",     1, ""}, {"MOV L,D", 1, ""},     {"MOV L,E",     1, ""},     {"MOV L,H", 1, ""},        {"MOV L,L",      1, ""},        {"MOV L,M", 1, ""},       {"MOV L,A", 1, ""}, // 0X60
/* 0X70 */  {"MOV M,B",     1, ""}, {"MOV M,C", 1, ""},    {"MOV M,D", 1, ""},    {"MOV M,E", 1, ""},         {"MOV M,H", 1, ""},        {"MOV M,L",     1, ""}, {"HLT",     1, ""},    {"MOV M,A", 1, ""}, {"MOV A,B",     1, ""}, {"MOV A,C",     1, ""}, {"MOV A,D", 1, ""},     {"MOV A,E",     1, ""},     {"MOV A,H", 1, ""},        {"MOV A,L",      1, ""},        {"MOV A,M", 1, ""},       {"MOV A,A", 1, ""}, // 0X70
/* 0X80 */  {"ADD B",       1, ""}, {"ADD C",   1, ""},    {"ADD D",   1, ""},    {"ADD E",   1, ""},         {"ADD H",   1, ""},        {"ADD L",       1, ""}, {"ADD M",   1, ""},    {"ADD A",   1, ""}, {"ADC B",       1, ""}, {"ADC C",       1, ""}, {"ADC D",   1, ""},     {"ADC E",       1, ""},     {"ADC H",   1, ""},        {"ADC L",        1, ""},        {"ADC M",   1, ""},       {"ADC A",   1, ""}, // 0X80
/* 0X90 */  {"SUB B",       1, ""}, {"SUB C",   1, ""},    {"SUB D",   1, ""},    {"SUB E",   1, ""},         {"SUB H",   1, ""},        {"SUB L",       1, ""}, {"SUB M",   1, ""},    {"SUB A",   1, ""}, {"SBB B",       1, ""}, {"SBB C",       1, ""}, {"SBB D",   1, ""},     {"SBB E",       1, ""},     {"SBB H",   1, ""},        {"SBB L",        1, ""},        {"SBB M",   1, ""},       {"SBB A",   1, ""}, // 0X90
/* 0XA0 */  {"ANA B",       1, ""}, {"ANA C",   1, ""},    {"ANA D",   1, ""},    {"ANA E",   1, ""},         {"ANA H",   1, ""},        {"ANA L",       1, ""}, {"ANA M",   1, ""},    {"ANA A",   1, ""}, {"XRA B",       1, ""}, {"XRA C",       1, ""}, {"XRA D",   1, ""},     {"XRA E",       1, ""},     {"XRA H",   1, ""},        {"XRA L",        1, ""},        {"XRA M",   1, ""},       {"XRA A",   1, ""}, // 0XA0
/* 0XB0 */  {"ORA B",       1, ""}, {"ORA C",   1, ""},    {"ORA D",   1, ""},    {"ORA E",   1, ""},         {"ORA H",   1, ""},        {"ORA L",       1, ""}, {"ORA M",   1, ""},    {"ORA A",   1, ""}, {"CMP B",       1, ""}, {"CMP C",       1, ""}, {"CMP D",   1, ""},     {"CMP E",       1, ""},     {"CMP H",   1, ""},        {"CMP L",        1, ""},        {"CMP M",   1, ""},       {"CMP A",   1, ""}, // 0XB0
/* 0XC0 */  {"RNZ",         1, ""}, {"POP B",   1, ""},    {"JNZ",     3, "ADR"}, {"JMP",     3, "ADR" },     {"CNZ",     3, "ADR"},     {"PUSH B",      1, ""}, {"ADI",  2, "D8"},     {"RST 0",   1, ""}, {"RZ",          1, ""}, {"RET",         1, ""}, {"JZ",      3, "ADR"},  {"(UNDOC) JMP", 1, ""},     {"CZ ",     3, "ADR"},     {"CALL ",        3, "ADR"},     {"ACI",     2, "D8"},     {"RST 1",   1, ""}, // 0XC0
/* 0XD0 */  {"RNC",         1, ""}, {"POP D",   1, ""},    {"JNC",     3, "ADR"}, {"OUT",     2, "PORT"},     {"CNC",     3, "ADR"},     {"PUSH D",      1, ""}, {"SUI",  2, "D8"},     {"RST 2",   1, ""}, {"RC",          1, ""}, {"(UNDOC) RET", 1, ""}, {"JC",      3, "ADR"},  {"IN ",         2, "PORT"}, {"CC ",     3, "ADR"},     {"(UNDOC) CALL", 1, ""},        {"SBI",     2, "D8"},     {"RST 3",   1, ""}, // 0XD0
/* 0XE0 */  {"RPO",         1, ""}, {"POP H",   1, ""},    {"JPO",     3, "ADR"}, {"XTHL",    1, ""},         {"CPO",     3, "ADR"},     {"PUSH H",      1, ""}, {"ANI",  2, "D8"},     {"RST 4",   1, ""}, {"RPE",         1, ""}, {"PCHL",        1, ""}, {"JPE",     3, "ADR"},  {"XCHG",        1, ""},     {"CPE ",    3, "ADR"},     {"(UNDOC) CALL", 1, ""},        {"XRI",     2, "D8"},     {"RST 5",   1, ""}, // 0XE0
/* 0XF0 */  {"RP",          1, ""}, {"POP PSW", 1, ""},    {"JP",      3, "ADR"}, {"DI",      1, ""},         {"CP",      3, "ADR"},     {"PUSH PSW",    1, ""}, {"ORI",  2, "D8"},     {"RST 6",   1, ""}, {"RM",          1, ""}, {"SPHL",        1, ""}, {"JM",      3, "ADR"},  {"EI",          1, ""},     {"CM ",     3, "ADR"},     {"(UNDOC) CALL", 1, ""},        {"CPI",     2, "D8"},     {"RST 7",   1, ""}, // 0XF0
            // 0x00         0x01                   0x02                   0x03                    0x04                   0x05                0x06               0x07            0x08            0x09            0x0A                   0x0B                    0x0C                   0x0D                   0x0E                  0x0F            //
        };
        // clang-format on

        return instructions[opcode];
    }

public:
    Disassembler(const u8* memory, u64 size) : _memory(memory), _memsize(size) {};

    DisassembleResult disassemble() {
        if (_prgCounter >= _memsize) {
            return {_prgCounter, 0, {0}, nullptr, true};
        }

        u8 opcode = memread(_prgCounter);

        DisassembleResult res = {};

        res.instruction = &getInstruction(opcode);

        if (_prgCounter + res.instruction->length > _memsize) {
            return { _prgCounter, 0, {0},  nullptr, true};
        }

        res.address = _prgCounter;
        res.bytesCount = res.instruction->length;
        res.bytes[0] = opcode;
        res.bytes[1] = (res.bytesCount > 1) ? memread(_prgCounter + 1) : 0;
        res.bytes[2] = (res.bytesCount > 2) ? memread(_prgCounter + 2) : 0;

        _prgCounter += res.bytesCount;

        return res;
    }

    void reset() { _prgCounter = 0; }

    u16 getPgCounter() const { return _prgCounter; }
private:
    const u8* _memory;
    u64 _memsize;
    u16 _prgCounter = 0x0000;

    u8 memread(u16 adr) { return _memory[adr]; }
};