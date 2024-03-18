#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <mutex>
#include <thread>
#include <vector>

#include <SFML/Audio.hpp>

// #define EMULATE_OLD_UMPK

#include "../core/disassembler.hpp"
#include "../core/dj.hpp"
#include "../core/umpk80.hpp"



#ifdef EMULATE_OLD_UMPK
#define OS_FILE "./data/old.bin"
#else
#define OS_FILE "./data/scaned-os.bin"
#endif

class Controller {
public:
    const uint16_t UMPK_ROM_SIZE = 0x800;  

public:
    Controller(class GuiApp& gui)
        : //_umpkThread(&Controller::_umpkWork, this),
          _disasm(nullptr, 0) 
    {
        
    }

    // ~Controller() { _umpkThread.detach(); }

    void decompileToFile(std::string filename, uint16_t fromAdr, uint16_t len);

    void onBtnStart();
    void onButtonStop();
    void onBtnNextCommand();
    void onBtnReset();

    void setUmpkKey(KeyboardKey key, bool value);
    bool getUmpkKeyState(KeyboardKey key) { return _umpk.getKeyState(key); }

    bool isUmpkRunning() { return !_isUmpkFreezed; }

    uint8_t getDisplayDigit(int digit) { return _umpk.getDisplayDigit(digit); }

    uint8_t port5Out() { return _umpk.port5OutGet(); }
    void port5In(uint8_t data);

    void setCpuFlags(CpuFlagsMapping flags);
    void setCpuProgramCounter(uint16_t value);
    void setCpuStackPointer(uint16_t sp);

    void setMemory(uint16_t index, uint8_t data);
    
    uint8_t getRegister(Cpu::Register reg) { return _umpk.getCpu().getRegister(reg); }
    void setRegister(Cpu::Register reg, uint8_t value) {
        _umpkMutex.lock();
        _umpk.getCpu().setRegister(reg, value);
        _umpkMutex.unlock();
    }

    std::vector<uint8_t> readBinaryFile(std::string path);

    void loadProgramToMemory(uint16_t position, std::vector<uint8_t> &program);
    
    Umpk80& umpk() { return _umpk; }

    const uint8_t* getRam() { return &(_umpk.getBus().ramFirst()); }
    const uint8_t* getRom() { return &(_umpk.getBus().romFirst()); }

private:
    Umpk80 _umpk;
    Disassembler _disasm;
    Dj dj;

    std::thread _umpkThread;
    std::mutex _umpkMutex;

    bool _isUmpkFreezed = true;
    bool _isUmpkWorking = true;

private:
    void _loadSystem();
    void _handleHooks(Cpu &cpu);
    void _umpkWork();
    void _copyTestToMemory(uint16_t startAdr, uint8_t *test, size_t size);
};

#endif // CONTROLLER_HPP
