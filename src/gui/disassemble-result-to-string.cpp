#include "disassemble-result-to-string.hpp"

std::string disassembleResultToString(const Disassembler::DisassembleResult& res) {
    switch (res.bytesCount) {
    case 1:
        return {res.instruction->mnemonic};
    case 2:
        return formatedString("%s %02Xh", res.instruction->mnemonic,
                              res.bytes[1]);
    case 3:
        return formatedString("%s %02X%02Xh", res.instruction->mnemonic,
                              res.bytes[2], res.bytes[1]);

    default:
        return "-";
    }
}