#include "view.hpp"

#include "controller.hpp"

int main() {
    View app;
    app.start();
    
    // Controller ctr;

    // ctr.onBtnStart();

    // Umpk80 umpk;

    // char os[0x800] = {0}; 

    // std::ifstream file(OS_FILE, std::ios::binary);

    // file.read(os, 0x800);
    // file.close();

    // umpk.loadOS((const uint8_t*)os);

    // // umpk.getCpu().setProgramCounter(0x447);
    // while (true) {
    //     Cpu& cpu = umpk.getCpu();
    //     // printf(
    //     //     "PG=%04x CMD=%02x A=%02x H=%02x L=%02x\n", 
    //     //     cpu.getProgramCounter(),
    //     //     cpu.getCommandRegister(),
    //     //     cpu.getRegister(Cpu::Register::A),
    //     //     cpu.getRegister(Cpu::Register::H),
    //     //     cpu.getRegister(Cpu::Register::L)
    //     // );

    //     umpk.tick();


    // }

    return 0;
}