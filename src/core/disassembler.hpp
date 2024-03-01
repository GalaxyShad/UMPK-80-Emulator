#pragma once 

#include <algorithm>
#include <cinttypes>
#include <string>
#include <vector>

#include "bus.hpp"

std::string toUpper(const std::string src) {
    std::string result = src;

    std::transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
}

class Disassembler {
public:
    struct Instruction {
        std::string     mnemonic;
        int             length;
        std::string     operand = "";
    };

    struct Result {
        std::vector<uint8_t> bytes;
        std::string          instruction;
    };

    static Instruction getInstruction(uint8_t opcode) {
        static const Instruction instructions[256] = {
            // 0x00         0x01                   0x02                   0x03                    0x04                   0x05                0x06               0x07            0x08            0x09            0x0A                   0x0B                    0x0C                   0x0D                   0x0E                  0x0F            //     
/* 0X00 */  {"NOP",     1}, {"LXI B,",  3, "D16"}, {"STAX B",  1},        {"INX B",   1},         {"INR B",   1},        {"DCR B",       1}, {"MVI B",   2},    {"RLC",     1}, {"UNKI",    1}, {"DAD B",   1}, {"LDAX B",  1},        {"DCX B",   1},         {"INR C",   1},        {"DCR C",   1},        {"MVI C",   2},       {"RRC",     1}, // 0X00
/* 0X01 */  {"UNKI",    1}, {"LXI D,",  3, "D16"}, {"STAX D",  1},        {"INX D",   1},         {"INR D",   1},        {"DCR D",       1}, {"MVI D",   2},    {"RAL",     1}, {"UNKI",    1}, {"DAD D",   1}, {"LDAX D",  1},        {"DCX D",   1},         {"INR E",   1},        {"DCR E",   1},        {"MVI E",   2},       {"RAR",     1}, // 0X10
/* 0X02 */  {"UNKI",    1}, {"LXI H,",  3, "D16"}, {"SHLD",    3, "ADR"}, {"INX H",   1},         {"INR H",   1},        {"DCR H",       1}, {"MVI H",   2},    {"DAA",     1}, {"UNKI",    1}, {"DAD H",   1}, {"LHLD",    3, "ADR"}, {"DCX H",   1},         {"INR L",   1},        {"DCR L",   1},        {"MVI L",   2},       {"CMA",     1}, // 0X20
/* 0X03 */  {"UNKI",    1}, {"LXI SP,", 3},        {"STA",     3, "ADR"}, {"INX SP",  1},         {"INR M",   1},        {"DCR M",       1}, {"MVI M",   2},    {"STC",     1}, {"UNKI",    1}, {"DAD SP",  1}, {"LDA",     3, "ADR"}, {"DCX SP",  1},         {"INR A",   1},        {"DCR A",   1},        {"MVI A",   2},       {"CMC",     1}, // 0X30
/* 0X04 */  {"MOV B,B", 1}, {"MOV B,C", 1},        {"MOV B,D", 1},        {"MOV B,E", 1},         {"MOV B,H", 1},        {"MOV B,L",     1}, {"MOV B,M", 1},    {"MOV B,A", 1}, {"MOV C,B", 1}, {"MOV C,C", 1}, {"MOV C,D", 1},        {"MOV C,E", 1},         {"MOV C,H", 1},        {"MOV C,L", 1},        {"MOV C,M", 1},       {"MOV C,A", 1}, // 0X40
/* 0X05 */  {"MOV D,B", 1}, {"MOV D,C", 1},        {"MOV D,D", 1},        {"MOV D,E", 1},         {"MOV D,H", 1},        {"MOV D,L",     1}, {"MOV D,M", 1},    {"MOV D,A", 1}, {"MOV E,B", 1}, {"MOV E,C", 1}, {"MOV E,D", 1},        {"MOV E,E", 1},         {"MOV E,H", 1},        {"MOV E,L", 1},        {"MOV E,M", 1},       {"MOV E,A", 1}, // 0X50
/* 0X06 */  {"MOV H,B", 1}, {"MOV H,C", 1},        {"MOV H,D", 1},        {"MOV H,E", 1},         {"MOV H,H", 1},        {"MOV H,L",     1}, {"MOV H,M", 1},    {"MOV H,A", 1}, {"MOV L,B", 1}, {"MOV L,C", 1}, {"MOV L,D", 1},        {"MOV L,E", 1},         {"MOV L,H", 1},        {"MOV L,L", 1},        {"MOV L,M", 1},       {"MOV L,A", 1}, // 0X60
/* 0X07 */  {"MOV M,B", 1}, {"MOV M,C", 1},        {"MOV M,D", 1},        {"MOV M,E", 1},         {"MOV M,H", 1},        {"MOV M,L",     1}, {"HLT",     1},    {"MOV M,A", 1}, {"MOV A,B", 1}, {"MOV A,C", 1}, {"MOV A,D", 1},        {"MOV A,E", 1},         {"MOV A,H", 1},        {"MOV A,L", 1},        {"MOV A,M", 1},       {"MOV A,A", 1}, // 0X70
/* 0X08 */  {"ADD B",   1}, {"ADD C",   1},        {"ADD D",   1},        {"ADD E",   1},         {"ADD H",   1},        {"ADD L",       1}, {"ADD M",   1},    {"ADD A",   1}, {"ADC B",   1}, {"ADC C",   1}, {"ADC D",   1},        {"ADC E",   1},         {"ADC H",   1},        {"ADC L",   1},        {"ADC M",   1},       {"ADC A",   1}, // 0X80
/* 0X09 */  {"SUB B",   1}, {"SUB C",   1},        {"SUB D",   1},        {"SUB E",   1},         {"SUB H",   1},        {"SUB L",       1}, {"SUB M",   1},    {"SUB A",   1}, {"SBB B",   1}, {"SBB C",   1}, {"SBB D",   1},        {"SBB E",   1},         {"SBB H",   1},        {"SBB L",   1},        {"SBB M",   1},       {"SBB A",   1}, // 0X90
/* 0X0A */  {"ANA B",   1}, {"ANA C",   1},        {"ANA D",   1},        {"ANA E",   1},         {"ANA H",   1},        {"ANA L",       1}, {"ANA M",   1},    {"ANA A",   1}, {"XRA B",   1}, {"XRA C",   1}, {"XRA D",   1},        {"XRA E",   1},         {"XRA H",   1},        {"XRA L",   1},        {"XRA M",   1},       {"XRA A",   1}, // 0XA0
/* 0X0B */  {"ORA B",   1}, {"ORA C",   1},        {"ORA D",   1},        {"ORA E",   1},         {"ORA H",   1},        {"ORA L",       1}, {"ORA M",   1},    {"ORA A",   1}, {"CMP B",   1}, {"CMP C",   1}, {"CMP D",   1},        {"CMP E",   1},         {"CMP H",   1},        {"CMP L",   1},        {"CMP M",   1},       {"CMP A",   1}, // 0XB0
/* 0X0C */  {"RNZ",     1}, {"POP B",   1},        {"JNZ",     3, "ADR"}, {"JMP",     3, "ADR" }, {"CNZ",     3, "ADR"}, {"PUSH B",      1}, {"ADI",  2, "D8"}, {"RST 0",   1}, {"RZ",      1}, {"RET",     1}, {"JZ",      3, "ADR"}, {"UNKI",    1},         {"CZ ",     3, "ADR"}, {"CALL ",   3, "ADR"}, {"ACI",     2, "D8"}, {"RST 1",   1}, // 0XC0
/* 0X0D */  {"RNC",     1}, {"POP D",   1},        {"JNC",     3, "ADR"}, {"OUT",     2, "PORT"}, {"CNC",     3, "ADR"}, {"PUSH D",      1}, {"SUI",  2, "D8"}, {"RST 2",   1}, {"RC",      1}, {"UNKI",    1}, {"JC",      3, "ADR"}, {"IN ",     2, "PORT"}, {"CC ",     3, "ADR"}, {"UNKI",    1},        {"SBI",     2, "D8"}, {"RST 3",   1}, // 0XD0
/* 0X0E */  {"RPO",     1}, {"POP H",   1},        {"JPO",     3, "ADR"}, {"XTHL",    1},         {"CPO",     3, "ADR"}, {"PUSH H",      1}, {"ANI",  2, "D8"}, {"RST 4",   1}, {"RPE",     1}, {"PCHL",    1}, {"JPE",     3, "ADR"}, {"XCHG",    1},         {"CPE ",    3, "ADR"}, {"UNKI",    1},        {"XRI",     2, "D8"}, {"RST 5",   1}, // 0XE0
/* 0X0F */  {"RP",      1}, {"POP PSW", 1},        {"JP",      3, "ADR"}, {"DI",      1},         {"CP",      3, "ADR"}, {"PUSH PSW",    1}, {"ORI",  2, "D8"}, {"RST 6",   1}, {"RM",      1}, {"SPHL",    1}, {"JM",      3, "ADR"}, {"EI",      1},         {"CM ",     3, "ADR"}, {"UNKI",    1},        {"CPI",     2, "D8"}, {"RST 7",   1}, // 0XF0
            // 0x00         0x01                   0x02                   0x03                    0x04                   0x05                0x06               0x07            0x08            0x09            0x0A                   0x0B                    0x0C                   0x0D                   0x0E                  0x0F            // 
        };

        return instructions[opcode];
    }

public:
    Disassembler(std::vector<uint8_t>& memory) : _memory(memory) {};

    std::string translateOld() {
        if (_prgCounter >= _memory.size()) return "EOF"; 

        uint8_t opcode = memRead(_prgCounter);

        Instruction instruction = getInstruction(opcode);

        instruction.mnemonic += " ";
        for (int i = 1; i < instruction.length; i++) {
            char value[8];
            sprintf(value, "%02x", memRead(_prgCounter + instruction.length-i));
            instruction.mnemonic += std::string(value);
        }

        _prgCounter += instruction.length;

        return toUpper(instruction.mnemonic) + (instruction.length != 1 ? "h" : "");
    }

    std::vector<uint8_t> getBytes() {
        std::vector<uint8_t> res;

        uint8_t opcode = memRead(_prgCounter);

        Instruction instruction = getInstruction(opcode);
        for (int i = 0; i < instruction.length; i++) {
            res.push_back(memRead(_prgCounter + i));
        }

        return res;
    }

    void reset() { _prgCounter = 0; } 
    void reset(uint16_t prgCounter) { _prgCounter = prgCounter; } 

    uint16_t getPgCounter() {
        return _prgCounter;
    }

    bool isEof() { return _prgCounter >= 0x1000; }

private:
    std::vector<uint8_t>& _memory;
    uint16_t _prgCounter = 0x0000;

    uint8_t memRead(uint16_t adr) {
        return _memory[adr];
    }
};