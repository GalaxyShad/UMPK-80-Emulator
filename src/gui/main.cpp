#include "gui-app.hpp"
#include "../core/cpu.hpp"
#include "../core/bus.hpp"
#include <cstdint>

enum {
            //  0x00    0x01     0x02    0x03    0x04    0x05      0x06    0x07    0x08    0x09    0x0A    0x0B    0x0C    0x0D    0x0E    0x0F        //     
    /* 0x00 */  NOP,    LXI_B,   STAX_B, INX_B,  INR_B,  DCR_B,    MVI_B,  RLC,    NOP_08, DAD_B,  LDAX_B, DCX_B,  INR_C,  DCR_C,  MVI_C,  RRC,    // 0x00
    /* 0x10 */  NOP_10, LXI_D,   STAX_D, INX_D,  INR_D,  DCR_D,    MVI_D,  RAL,    NOP_18, DAD_D,  LDAX_D, DCX_D,  INR_E,  DCR_E,  MVI_E,  RAR,    // 0x10
    /* 0x20 */  NOP_20, LXI_H,   SHLD,   INX_H,  INR_H,  DCR_H,    MVI_H,  DAA,    NOP_28, DAD_H,  LHLD,   DCX_H,  INR_L,  DCR_L,  MVI_L,  CMA,    // 0x20
    /* 0x30 */  NOP_30, LXI_SP,  STA,    INX_SP, INR_M,  DCR_M,    MVI_M,  STC,    NOP_38, DAD_SP, LDA,    DCX_SP, INR_A,  DCR_A,  MVI_A,  CMC,    // 0x30
    /* 0x40 */  MOV_BB, MOV_BC,  MOV_BD, MOV_BE, MOV_BH, MOV_BL,   MOV_BP, MOV_BA, MOV_CB, MOV_CC, MOV_CD, MOV_CE, MOV_CH, MOV_CL, MOV_CP, MOV_CA, // 0x40
    /* 0x50 */  MOV_DB, MOV_DC,  MOV_DD, MOV_DE, MOV_DH, MOV_DL,   MOV_DP, MOV_DA, MOV_EB, MOV_EC, MOV_ED, MOV_EE, MOV_EH, MOV_EL, MOV_EP, MOV_EA, // 0x50
    /* 0x60 */  MOV_HB, MOV_HC,  MOV_HD, MOV_HE, MOV_HH, MOV_HL,   MOV_HP, MOV_HA, MOV_IB, MOV_IC, MOV_ID, MOV_IE, MOV_IH, MOV_IL, MOV_IP, MOV_IA, // 0x60
    /* 0x70 */  MOV_MB, MOV_MC,  MOV_MD, MOV_ME, MOV_MH, MOV_ML,   MOV_MP, MOV_MA, MOV_SB, MOV_SC, MOV_SD, MOV_SE, MOV_SH, MOV_SL, MOV_SP, HLT,    // 0x70
    /* 0x80 */  ADD_B,  ADD_C,   ADD_D,  ADD_E,  ADD_H,  ADD_L,    ADD_M,  ADD_A,  ADC_B,  ADC_C,  ADC_D,  ADC_E,  ADC_H,  ADC_L,  ADC_M,  ADC_A,  // 0x80
    /* 0x90 */  SUB_B,  SUB_C,   SUB_D,  SUB_E,  SUB_H,  SUB_L,    SUB_M,  SUB_A,  SBB_B,  SBB_C,  SBB_D,  SBB_E,  SBB_H,  SBB_L,  SBB_M,  SBB_A,  // 0x90
    /* 0xA0 */  ANA_B,  ANA_C,   ANA_D,  ANA_E,  ANA_H,  ANA_L,    ANA_M,  ANA_A,  XRA_B,  XRA_C,  XRA_D,  XRA_E,  XRA_H,  XRA_L,  XRA_M,  XRA_A,  // 0xA0
    /* 0xB0 */  ORA_B,  ORA_C,   ORA_D,  ORA_E,  ORA_H,  ORA_L,    ORA_M,  ORA_A,  CMP_B,  CMP_C,  CMP_D,  CMP_E,  CMP_H,  CMP_L,  CMP_M,  CMP_A,  // 0xB0
    /* 0xC0 */  RNZ,    POP_B,   JNZ,    JMP,    CNZ,    PUSH_B,   ADI,    RST_0,  RZ,     RET,    JZ,     _CB,    CZ,     CALL,   ACI,    RST_1,  // 0xC0
    /* 0xD0 */  RNC,    POP_D,   JNC,    OUT,    CNC,    PUSH_D,   SUI,    RST_2,  RC,     _D9,    JC,     IN,     CC,     _DD,    SBI,    RST_3,  // 0xD0
    /* 0xE0 */  RPO,    POP_H,   JPO,    XTHL,   CPO,    PUSH_H,   ANI,    RST_4,  RPE,    PCHL,   JPE,    XCHG,   CPE,    _ED,    XRI,    RST_5,  // 0xE0
    /* 0xF0 */  RP,     POP_PSW, JP,     DI,     CP,     PUSH_PSW, ORI,    RST_6,  RM,     SPHL,   JM,     EI,     CM,     _FD,    CPI,    RST_7,  // 0xF0
};

void test(uint8_t in, uint8_t expected) {
    printf("[%s] Got = %02X; Expected = %02X\n\n", in == expected ? "OK" : "FAIL", in, expected);
}

uint8_t runDaa(uint8_t a, uint8_t b) {
    Bus bus;

    uint8_t rom[] = {
        MVI_A, a,
        MVI_B, b,
        ADD_B,
        DAA
    };

    size_t romSize = sizeof(rom) / sizeof(rom[0]);

    bus.loadRom(rom, romSize);
    
    Cpu i8080(bus);
    for (int i = 0; i < 4; i++) {
        i8080.tick();
        printf("[LOG] A = %02X B = %02X ; FLAG_AC = %01X\n", i8080.A(), i8080.B(), i8080.getFlags().auxcarry);
    }

    return i8080.A();
}

void runTestDAA() {
    test(runDaa(0x33, 0x49), 0x82);
    test(runDaa(0x38, 0x45), 0x83);
    test(runDaa(0x38, 0x41), 0x79);
    test(runDaa(0x83, 0x54), 0x37);
    test(runDaa(0x88, 0x44), 0x32);
}

int main() {
#ifdef DEBUG
    runTestDAA();
#endif

    GuiApp app;
    app.start();
    return 0;
}