#pragma once

#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>
#include <fstream>

#include <SFML/Audio.hpp>
#include <cmath>

// #define EMULATE_OLD_UMPK

#include "umpk80.hpp"
#include "disassembler.hpp"
#include "dj.hpp"
#include "viewcontrol.hpp"

#ifdef EMULATE_OLD_UMPK
    #define OS_FILE "../data/old.bin"
#else
    // #define OS_FILE "../data/scaned-os.bin"
    #define OS_FILE "scaned-os.bin"
#endif

class Controller {
    public:
        Controller(ViewControl& vc) : 
            _umpkThread(_umpkWork, this),
            view(vc)
        {}

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

        uint8_t getDisplayDigit(int digit) {
            // return 0;
            return _umpk.getDisplayDigit(digit);
        }

        void onUmpkKeySet(Keyboard::Key key, bool value) {
            _umpkMutex.lock();
            (value == true) ? _umpk.pressKey(key) : _umpk.releaseKey(key);
            _umpkMutex.unlock();
        }

        uint8_t port5Out() {
            // return 0;
            return _umpk.port5OutGet();
        }

        void port5In(uint8_t data) {
            // _umpkMutex.lock();
            // _umpk.port5InSet(data);
            // _umpkMutex.unlock();
        }

        Umpk80& getUmpk() { return _umpk; }

        // void onSetProgramCounter(uint16_t value) {
        //     _cpu.setProgramCounter(value);
        // }

        // void loadTest(uint8_t testNum) {
        //     _isCpuRunning = false;

        //     // switch(testNum) {
        //     //     case 1: _testLab1(); break;
        //     //     case 2: _testLab2(); break;
        //     //     case 3: _testLab3(); break;
        //     //     case 4: _testLab4(); break;
        //     //     case 5: _testLab5(); break;
        //     //     case 6: _testLab6(); break;
        //     // }

        //     switch(testNum) {
        //         case 1: _loadProgramFromFile("1.i8080asm.bin"); break;
        //         case 2: _loadProgramFromFile("2.i8080asm.bin"); break;
        //         case 3: _loadProgramFromFile("3.i8080asm.bin"); break;
        //         case 4: _loadProgramFromFile("4.i8080asm.bin"); break;
        //         case 5: _loadProgramFromFile("5.i8080asm.bin"); break;
        //         case 6: _loadProgramFromFile("6.i8080asm.bin"); break;
        //     }

        //     _isCpuRunning = true;
        // }


    private:
        Umpk80 _umpk;
        Dj dj;

        std::thread _umpkThread;
        std::mutex  _umpkMutex;

        bool _isUmpkFreezed = true; 

        ViewControl& view;


        void _loadSystem() {
            char os[0x800] = {0}; 

            std::ifstream file(OS_FILE, std::ios::binary);

            file.read(os, 0x800);
            file.close();

            _umpk.loadOS((const uint8_t*)os);
        }

        void _handleHooks(Cpu& cpu) {
            uint16_t pgCounter = cpu.getProgramCounter();

            const uint16_t SOUND_FUNC_ADR = 0x0447;
            const uint16_t DELAY_FUNC_ADR = 0x0506;

            if (pgCounter == SOUND_FUNC_ADR) {
                uint8_t duration  = cpu.getRegister(Cpu::Register::D);
                uint8_t frequency = 0xFF - cpu.getRegister(Cpu::Register::B);

                dj.tone(frequency*2, duration * 130);
            }

            if (pgCounter == DELAY_FUNC_ADR) {
                uint16_t delay = cpu.getRegister(Cpu::Register::B);
                delay = (delay << 8) | cpu.getRegister(Cpu::Register::C);
                
                for (int i = 0; i < 0xF000 * delay; i++);
            }
        }
         
        void _umpkWork() {
            _loadSystem();

            while (true) {
                if (_isUmpkFreezed) continue;
                _umpkMutex.lock();
                try {
                    _umpk.tick();
                } catch (const std::logic_error le) {
                    view.errorMessageBox(le.what());
                    _umpk.restart();
                    _umpk.tick();
                }
                _handleHooks(_umpk.getCpu());
                _umpkMutex.unlock();
            }
        }

        void _copyTestToMemory(uint16_t startAdr, uint8_t* test, size_t size) {
            // for (size_t i = 0; i < size; i++) {
            //     _bus.write(startAdr + i, test[i]);
            // }
        }


        void _testLab1() {
            uint8_t data[] = {
                0x21,
                0x00,
                0x09,
                0x46,
                0x24,
                0x7D,
                0xE6,
                0x08,
                0x4D,
                0xCA,
                0x10,
                0x08,
                0x7D,
                0xEE,
                0x07,
                0x6F,
                0x70,
                0x69,
                0x25,
                0x23,
                0x7D,
                0xE6,
                0xF0,
                0xCA,
                0x03,
                0x08,
                0xCF,
            };

            _copyTestToMemory(0x0800, data, 27);
        }

        void _testLab2() {
            uint8_t data[] = {
                0xDB,
                0x05,
                0xE6,
                0x0E,
                0xFE,
                0x02,
                0xDB,
                0x05,
                0xCA,
                0x14,
                0x08,
                0xE6,
                0xBF,
                0xF6,
                0x20,
                0xEE,
                0x10,
                0xC3,
                0x1A,
                0x08,
                0xE6,
                0xFE,
                0xF6,
                0x40,
                0xEE,
                0x80,
                0xD3,
                0x05,
                0xC3,
                0x00,
                0x08,
            };

            _copyTestToMemory(0x0800, data, 31);
        }

        void _testLab3() {
            uint8_t data[] = {
                0x21,
                0x02,
                0x09,
                0x4E,
                0x2B,
                0x7E,
                0x2B,
                0x86,
                0xDA,
                0x12,
                0x08,
                0x91,
                0xDA,
                0x23,
                0x08,
                0xC3,
                0x16,
                0x08,
                0x91,
                0xD2,
                0x1E,
                0x08,
                0xFE,
                0xF7,
                0xDA,
                0x23,
                0x08,
                0xC3,
                0x28,
                0x08,
                0xE6,
                0xFE,
                0xCA,
                0x28,
                0x08,
                0x3E,
                0x70,
                0xC3,
                0x2A,
                0x08,
                0x3E,
                0x07,
                0xD3,
                0x05,
                0xCF,
            };

            _copyTestToMemory(0x0800, data, 45);
        }

        void _testLab4() {
            uint8_t data[] = {
                0xCD,
                0x55,
                0x0C,
                0x78,
                0x77,
                0x2F,
                0x47,
                0xCD,
                0x4B,
                0x0C,
                0xC2,
                0x03,
                0x0C,
                0xCF,
                0xCD,
                0x55,
                0x0C,
                0x78,
                0xBE,
                0xC2,
                0x22,
                0x0C,
                0x2F,
                0x47,
                0xCD,
                0x4B,
                0x0C,
                0xC2,
                0x11,
                0x0C,
                0xCD,
                0xBA,
                0x05,
                0xCF,
                0x46,
                0xEB,
                0x21,
                0xF5,
                0x0B,
                0x4A,
                0xCD,
                0x3C,
                0x0C,
                0x4B,
                0xCD,
                0x3C,
                0x0C,
                0x48,
                0xCD,
                0x3C,
                0x0C,
                0xCD,
                0xE9,
                0x01,
                0xCD,
                0xC8,
                0x01,
                0xC3,
                0x36,
                0x0C,
                0x79,
                0xE6,
                0xF0,
                0x0F,
                0x0F,
                0x0F,
                0x0F,
                0x77,
                0x2B,
                0x79,
                0xE6,
                0x0F,
                0x77,
                0x2B,
                0xC9,

                0x23,
                0x7C,
                0xFE,
                0x0A,
                0xC0,
                0x7D,
                0xFE,
                0x70,
                0xC0,
                0xC9,

                0x21,
                0x00,
                0x08,
                0x06,
                0x55,
                0xC9,
            };

            _copyTestToMemory(0x0C00, data, 91);
        }

        void _testLab5() {
            uint8_t data[] = {
                0xCD,
                0xBF,
                0x08,
                0x21,
                0x06,
                0x09,
                0x06,
                0x0A,
                0x16,
                0xFF,
                0xCD,
                0x31,
                0x08,
                0xCD,
                0x9A,
                0x08,
                0x7A,
                0xFE,
                0xFF,
                0xC2,
                0x1D,
                0x08,
                0x0E,
                0x0C,
                0x23,
                0x0D,
                0xC2,
                0x18,
                0x08,
                0x7E,
                0xFE,
                0xFF,
                0xC2,
                0x26,
                0x08,
                0x21,
                0x06,
                0x09,
                0xFE,
                0x44,
                0xC2,
                0x0A,
                0x08,
                0x21,
                0x60,
                0x09,
                0xC3,
                0x0A,
                0x08,
                0xCD,
                0x59,
                0x08,
                0xFE,
                0x02,
                0xC2,
                0x3F,
                0x08,
                0x04,
                0xD2,
                0x3F,
                0x08,
                0x06,
                0xFF,
                0xFE,
                0x09,
                0xC2,
                0x4A,
                0x08,
                0x05,
                0xD2,
                0x4A,
                0x08,
                0x06,
                0x00,
                0xFE,
                0x04,
                0xC2,
                0x51,
                0x08,
                0x16,
                0xFF,
                0xFE,
                0x06,
                0xC2,
                0x58,
                0x08,
                0x16,
                0x00,
                0xC9,
                0x3E,
                0xEF,
                0xD3,
                0x07,
                0xDB,
                0x06,
                0xFE,
                0xFE,
                0xC2,
                0x67,
                0x08,
                0x3E,
                0x04,
                0xC9,
                0xFE,
                0xFB,
                0xC2,
                0x6F,
                0x08,
                0x3E,
                0x06,
                0xC9,
                0x3E,
                0xF7,
                0xD3,
                0x07,
                0xDB,
                0x06,
                0xFE,
                0xFD,
                0xC2,
                0x7D,
                0x08,
                0x3E,
                0x02,
                0xC9,
                0x3E,
                0xDF,
                0xD3,
                0x07,
                0xDB,
                0x06,
                0xFE,
                0xFB,
                0xC2,
                0x8B,
                0x08,
                0x3E,
                0x09,
                0xC9,
                0x3E,
                0xFF,
                0xC9,
                0x58,
                0xCD,
                0x29,
                0x04,
                0xCD,
                0xC8,
                0x01,
                0x1D,
                0xC2,
                0x8F,
                0x08,
                0xC9,
                0x7E,
                0x32,
                0xFF,
                0x0B,
                0x2B,
                0x7E,
                0x32,
                0xFE,
                0x0B,
                0x2B,
                0x7E,
                0x32,
                0xFD,
                0x0B,
                0x2B,
                0x7E,
                0x32,
                0xFC,
                0x0B,
                0x2B,
                0x7E,
                0x32,
                0xFB,
                0x0B,
                0x2B,
                0x7E,
                0x32,
                0xFA,
                0x0B,
                0x2B,
                0xCD,
                0xC8,
                0x01,
                0xCD,
                0x8E,
                0x08,
                0xC9,
                0x21,
                0x00,
                0x09,
                0x36,
                0x44,
                0x23,
                0x1E,
                0x07,
                0x3E,
                0x01,
                0x0E,
                0x06,
                0x77,
                0x19,
                0x0D,
                0xC2,
                0xCB,
                0x08,
                0x2B,
                0x1B,
                0x36,
                0x20,
                0x19,
                0x36,
                0x10,
                0x19,
                0x36,
                0x08,
                0x1B,
                0x3E,
                0x08,
                0x0E,
                0x06,
                0x77,
                0x19,
                0x0D,
                0xC2,
                0xE0,
                0x08,
                0x23,
                0x13,
                0x36,
                0x04,
                0x19,
                0x36,
                0x02,
                0x19,
                0x19,
                0x2B,
                0x36,
                0xFF,
                0xC9,
            };

            _copyTestToMemory(0x0800, data, 243);
        }

        void _testLab6() {
            uint8_t data[] = {
                0x01,
                0x05,
                0x09,
                0xCD,
                0x53,
                0x08,
                0xDB,
                0x05,
                0xE6,
                0x80,
                0xCA,
                0x00,
                0x08,
                0xDB,
                0x05,
                0xE6,
                0x20,
                0xCA,
                0x1D,
                0x08,
                0x01,
                0x0B,
                0x09,
                0xCD,
                0x53,
                0x08,
                0xCD,
                0x40,
                0x08,
                0xDB,
                0x05,
                0xE6,
                0x02,
                0xCA,
                0x2D,
                0x08,
                0x01,
                0x11,
                0x09,
                0xCD,
                0x53,
                0x08,
                0xCD,
                0x40,
                0x08,
                0xDB,
                0x05,
                0xE6,
                0x08,
                0xCA,
                0x00,
                0x08,
                0x01,
                0x17,
                0x09,
                0xCD,
                0x53,
                0x08,
                0xCD,
                0x40,
                0x08,
                0xC3,
                0x00,
                0x08,
                0x06,
                0x02,
                0x0E,
                0xFF,
                0xCD,
                0x29,
                0x04,
                0xCD,
                0xC8,
                0x01,
                0x0D,
                0xC2,
                0x44,
                0x08,
                0x05,
                0xC2,
                0x44,
                0x08,
                0xC9,
                0x21,
                0xFA,
                0x0B,
                0x0A,
                0x77,
                0x0B,
                0x2C,
                0xC2,
                0x56,
                0x08,
                0xCD,
                0xC8,
                0x01,
                0xC9,
            };

            uint8_t dataSet[] = {
                0x39,
                0x6E,
                0x73,
                0x31,
                0x6E,
                0x00,
                0x6E,
                0x66,
                0x79,
                0x7D,
                0x77,
                0x00,
                0x7D,
                0x3F,
                0x73,
                0x79,
                0xBE,
                0x00,
                0x31,
                0x73,
                0x6E,
                0x37,
                0x37,
                0x77,
            };

            _copyTestToMemory(0x0800, data,    97); 
            _copyTestToMemory(0x0900, dataSet, 24); 
        }
};

