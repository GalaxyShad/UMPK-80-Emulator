#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <mutex>
#include <thread>
#include <vector>

#include <SFML/Audio.hpp>

// #define EMULATE_OLD_UMPK

#include "../core/disassembler.hpp"
#include "../core/umpk80.hpp"
#include "dj.hpp"

#include "gui-app-base.hpp"

#ifdef EMULATE_OLD_UMPK
#define OS_FILE "./data/old.bin"
#else
#define OS_FILE "./data/scaned-os-fixed.bin"
#endif

#include <iostream>

struct PrinterSoundPlayer : public ISoundPlayerObserver {
    void tone(const i64 *samples, int samplesSize) override {
        ui_sound::tone(std::vector<i64>(samples, samples + samplesSize));
    }
};

class Controller {
public:
    const uint16_t UMPK_ROM_SIZE = 0x800;

public:
    Controller(GuiAppBase &gui)
        : _umpkThread(&Controller::_umpkWork, this), _disasm(nullptr, 0),
          _gui(gui) {}

    ~Controller() {
        _umpkMutex.lock();
        _isUmpkWorking = false;
        _umpkMutex.unlock();
        _umpkThread.join();
    }

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

    uint16_t getSystemPG() {
        uint16_t high = _umpk.getBus().memoryRead(0xBBF);
        uint8_t low = _umpk.getBus().memoryRead(0xBBE);

        return (high << 8) | low;
    }

    void setCommand(uint8_t cmd) {
        uint16_t adr = getSystemPG();
        setMemory(adr, cmd);
    }

    uint8_t getRegister(Cpu::Register reg) {
        return _umpk.getCpu().getRegister(reg);
    }

    void setRegister(Cpu::Register reg, uint8_t value) {
        _umpkMutex.lock();
        _umpk.getCpu().setRegister(reg, value);
        _umpkMutex.unlock();
    }

    std::vector<uint8_t> readBinaryFile(std::string path);

    void loadProgramToMemory(uint16_t position, std::vector<uint8_t> &program);

    Umpk80 &umpk() { return _umpk; }

    const uint8_t *getRam() { return &(_umpk.getBus().ramFirst()); }
    const uint8_t *getRom() { return &(_umpk.getBus().romFirst()); }

    // FIXME
    int breakpoint = -1;

private:
    GuiAppBase &_gui;

    Umpk80 _umpk;
    Disassembler _disasm;

    std::thread _umpkThread;
    std::mutex _umpkMutex;

    PrinterSoundPlayer soundPlayer_;

    bool _isUmpkFreezed = true;
    bool _isUmpkWorking = true;

private:
    void _loadSystem();
    void _handleHooks(Cpu &cpu);
    void _umpkWork();
    void _copyTestToMemory(uint16_t startAdr, uint8_t *test, size_t size);
};

#endif // CONTROLLER_HPP
