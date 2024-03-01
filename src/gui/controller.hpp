#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>


#include <SFML/Audio.hpp>
#include <cmath>

// #define EMULATE_OLD_UMPK

#include "../core/disassembler.hpp"
#include "../core/dj.hpp"
#include "../core/umpk80.hpp"

#include "gui-app.hpp"


#ifdef EMULATE_OLD_UMPK
#define OS_FILE "./data/old.bin"
#else
#define OS_FILE "./data/scaned-os.bin"
#endif

class Controller {
public:
    Controller(class GuiApp& gui)
        : _umpkThread(&Controller::_umpkWork, this),
          _disasm(*(new std::vector<uint8_t>())) {}

    ~Controller() { _umpkThread.detach(); }

    void decompileToFile(std::string filename, uint16_t fromAdr, uint16_t len) {
        std::ofstream stream(filename);

        _disasm.reset(fromAdr);

        for (int i = 0; i < len; i++) {
            std::vector<uint8_t> mc = _disasm.getBytes();
            uint16_t adr = _disasm.getPgCounter();

            std::string instr = _disasm.translateOld();

            stream << std::hex << std::uppercase << std::setw(4)
                   << std::setfill('0') << adr << " | ";

            for (int j = 0; j < 3; j++) {
                if (j == 1)
                    stream << ' ';

                if (j < mc.size()) {
                    stream << std::hex << std::uppercase << std::setw(2)
                           << std::setfill('0') << (int)mc[j];
                } else {
                    stream << "  ";
                }
            }

            stream << " | " << instr << '\n';
        }

        stream.close();
    }

    void onBtnStart() {
        _umpkMutex.lock();
        _isUmpkFreezed = false;
        _umpkMutex.unlock();
    }

    void onButtonStop() {
        _umpkMutex.lock();
        _isUmpkFreezed = true;
        _umpkMutex.unlock();
    }

    void onBtnNextCommand() {
        _umpkMutex.lock();
        _umpk.tick();
        _umpkMutex.unlock();
    }

    void onBtnReset() {
        _umpkMutex.lock();
        _umpk.restart();
        _umpkMutex.unlock();
    }

    bool isUmpkRunning() { return !_isUmpkFreezed; }

    uint8_t getDisplayDigit(int digit) { return _umpk.getDisplayDigit(digit); }

    void onUmpkKeySet(Keyboard::Key key, bool value) {
        _umpkMutex.lock();
        (value == true) ? _umpk.pressKey(key) : _umpk.releaseKey(key);
        _umpkMutex.unlock();
    }

    uint8_t port5Out() { return _umpk.port5OutGet(); }

    void port5In(uint8_t data) {
        _umpkMutex.lock();
        _umpk.port5InSet(data);
        _umpkMutex.unlock();
    }

    Umpk80 &getUmpk() { return _umpk; }
    Disassembler &getDisasm() { return _disasm; }

    void onSetProgramCounter(uint16_t value) {
        _umpkMutex.lock();
        _umpk.getCpu().setProgramCounter(value);
        _umpkMutex.unlock();
    }

    std::vector<uint8_t> readBinaryFile(std::string path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file " + path);
        }

        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data(fileSize);
        if (!file.read(reinterpret_cast<char *>(data.data()), fileSize)) {
            throw std::runtime_error("Failed to read file " + path);
        }
        file.close();

        return data;
    }

    void loadProgramToMemory(uint16_t position, std::vector<uint8_t> &program) {
        _umpkMutex.lock();
        for (size_t i = 0; i < program.size(); i++) {
            _umpk.getBus().memoryWrite(position + i, program[i]);
        }
        _umpk.getCpu().interruptRst(7);
        _umpkMutex.unlock();
    }

private:
    Umpk80 _umpk;
    Disassembler _disasm;
    Dj dj;

    std::thread _umpkThread;
    std::mutex _umpkMutex;

    bool _isUmpkFreezed = true;

    void _loadSystem() {
        char os[0x800] = {0};

        std::ifstream file(OS_FILE, std::ios::binary);

        file.read(os, 0x800);
        file.close();

        _umpk.loadOS((const uint8_t *)os);
    }

    void _handleHooks(Cpu &cpu) {
        uint16_t pgCounter = cpu.getProgramCounter();

        const uint16_t SOUND_FUNC_ADR = 0x0447;
        const uint16_t DELAY_FUNC_ADR = 0x0506;

        if (pgCounter == SOUND_FUNC_ADR) {
            uint8_t duration = cpu.getRegister(Cpu::Register::D);
            uint8_t frequency = 0xFF - cpu.getRegister(Cpu::Register::B);

            dj.tone(frequency * 2, duration * 130);
        }

        if (pgCounter == DELAY_FUNC_ADR) {
            uint16_t delay = cpu.getRegister(Cpu::Register::B);
            delay = (delay << 8) | cpu.getRegister(Cpu::Register::C);

            for (int i = 0; i < 0xF000 * delay; i++)
                ;
        }
    }

    void _umpkWork() {
        _loadSystem();

        while (true) {
            if (_isUmpkFreezed)
                continue;
            _umpkMutex.lock();

            try {
                _umpk.tick();
            } catch (const std::logic_error le) {
                // view.errorMessageBox(std::string(le.what()));
                _umpk.getCpu().interruptRst(7);
            }

            _handleHooks(_umpk.getCpu());
            _umpkMutex.unlock();
        }
    }

    void _copyTestToMemory(uint16_t startAdr, uint8_t *test, size_t size) {
        for (size_t i = 0; i < size; i++) {
            _umpk.getBus().memoryWrite(startAdr + i, test[i]);
        }
    }
};
