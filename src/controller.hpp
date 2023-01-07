#pragma once

#include <thread>
#include <chrono>
#include <iostream>

#include "bus.hpp"
#include "cpu.hpp"
#include "display.hpp"
#include "disassembler.hpp"
#include "keyboard.hpp"

class Controller {
    public:
        Controller() : 
            _cpu(_bus), _display(_bus), _disasm(_bus), _keyboard(_bus),
            _cpuThread(_cpuWork, this)
        {
            _loadSystem();
        };

        void onBtnStart() {
            _isCpuRunning = true;
        }
        
        void onButtonStop() {
            _isCpuRunning = false;
        }

        void onBtnNextCommand() {
            _tick();
        }

        void onBtnReset() {
            _isCpuRunning = false;
            _cpu.setProgramCounter(0x0000);
            _isCpuRunning = true;
        }

        void onSetProgramCounter(uint16_t value) {
            _cpu.setProgramCounter(value);
        }

        void loadTest(uint8_t testNum) {
            _isCpuRunning = false;

            switch(testNum) {
                case 1: _testLab1(); break;
                case 2: _testLab2(); break;
                case 3: _testLab3(); break;
                case 4: _testLab4(); break;
                case 5: _testLab5(); break;
                case 6: _testLab6(); break;
            }

            _isCpuRunning = true;
        }

        Cpu& getCpu()                       { return _cpu;      }
        Bus& getBus()                       { return _bus;      }
        Display& getDisplay()               { return _display;  }
        Disassembler& getDisassembler()     { return _disasm;   }
        Keyboard&   getKeyboard()           { return _keyboard; }

    private:
        Cpu _cpu;
        Display _display;
        Bus _bus;
        Disassembler _disasm;
        Keyboard _keyboard;


        std::thread _cpuThread;

        bool _isCpuRunning = false;

        void _loadSystem() {
            // FILE* file = fopen("../data/old.bin", "rb");
            FILE* file = fopen("../data/os.bin", "rb");

            if (!file) {
                printf("Oh no, i can't open de file :(((((((");
            }

            uint8_t data = 0x00;
            int i = 0;
            while (fread(&data, 1, 1, file) == 1) {
                _bus.write(i, data);
                i++;
            }
            fclose(file);
        }

        void _tick() {
            _cpu.tick();
            _display.update();
            _keyboard.update();
        }

        void _cpuWork() {

            while (true) {
                // if (_cpu.getProgramCounter() == 0x3C2)
                //     getchar();

                if (_isCpuRunning)
                    _tick();

                std::this_thread::sleep_for(std::chrono::microseconds(700));
            }
        }

        void _copyTestToMemory(uint16_t startAdr, uint8_t* test, size_t size) {
            for (size_t i = 0; i < size; i++) {
                _bus.write(startAdr + i, test[i]);
            }
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
