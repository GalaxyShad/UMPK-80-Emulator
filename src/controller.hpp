#pragma once

#include <thread>
#include <chrono>
#include <iostream>

#include <SFML/Audio.hpp>
#include <cmath>

// #define EMULATE_OLD_UMPK

#include "bus.hpp"
#include "cpu.hpp"
#include "display.hpp"
#include "disassembler.hpp"
#include "keyboard.hpp"

#ifdef EMULATE_OLD_UMPK
    #define OS_FILE "../data/old.bin"
#else
    #define OS_FILE "../data/scaned-os.bin"
#endif


#define TWOPI 6.283185307

short SquareWave(double time, double freq, double amp) {
    if (freq == 0) return 0;


    short result = 0;
    int tpc = 44100 / freq; // ticks per cycle
    int cyclepart = int(time) % tpc;
    int halfcycle = tpc / 2;
    short amplitude = 32767 * amp;
    if (cyclepart < halfcycle) {
        result = amplitude;
    }
    return result;
}

short SineWave(double time, double freq, double amp) {
    short result;
    double tpc = 44100 / freq; // ticks per cycle
    double cycles = time / tpc;
    double rad = TWOPI * cycles;
    short amplitude = 32767 * amp;
    result = amplitude * sin(rad);
    return result;
}


class Controller {
    public:
        Controller() : 
            _cpu(_bus), _display(_bus), _disasm(_bus), _keyboard(_bus),
            _cpuThread(_cpuWork, this)
        {
            _loadSystem();
            // _greenHill();

            // _disasm.reset(0x055A);
            // for (int i = 0; i < 256; i++)
            //     std::cout << _disasm.translate() << std::endl;
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

        // Sound
        sf::SoundBuffer sndBuffer;
        int sndBufferCounter = 0;
        std::vector<sf::Int16> samples;
        sf::Sound sound;

        bool _isScnaning04Port = false;


        std::thread _cpuThread;

        bool _isCpuRunning = false;

        void _greenHill() {
            #define NOTE_B0  31
            #define NOTE_C1  33
            #define NOTE_CS1 35
            #define NOTE_D1  37
            #define NOTE_DS1 39
            #define NOTE_E1  41
            #define NOTE_F1  44
            #define NOTE_FS1 46
            #define NOTE_G1  49
            #define NOTE_GS1 52
            #define NOTE_A1  55
            #define NOTE_AS1 58
            #define NOTE_B1  62
            #define NOTE_C2  65
            #define NOTE_CS2 69
            #define NOTE_D2  73
            #define NOTE_DS2 78
            #define NOTE_E2  82
            #define NOTE_F2  87
            #define NOTE_FS2 93
            #define NOTE_G2  98
            #define NOTE_GS2 104
            #define NOTE_A2  110
            #define NOTE_AS2 117
            #define NOTE_B2  123
            #define NOTE_C3  131
            #define NOTE_CS3 139
            #define NOTE_D3  147
            #define NOTE_DS3 156
            #define NOTE_E3  165
            #define NOTE_F3  175
            #define NOTE_FS3 185
            #define NOTE_G3  196
            #define NOTE_GS3 208
            #define NOTE_A3  220
            #define NOTE_AS3 233
            #define NOTE_B3  247
            #define NOTE_C4  262
            #define NOTE_CS4 277
            #define NOTE_D4  294
            #define NOTE_DS4 311
            #define NOTE_E4  330
            #define NOTE_F4  349
            #define NOTE_FS4 370
            #define NOTE_G4  392
            #define NOTE_GS4 415
            #define NOTE_A4  440
            #define NOTE_AS4 466
            #define NOTE_B4  494
            #define NOTE_C5  523
            #define NOTE_CS5 554
            #define NOTE_D5  587
            #define NOTE_DS5 622
            #define NOTE_E5  659
            #define NOTE_F5  698
            #define NOTE_FS5 740
            #define NOTE_G5  784
            #define NOTE_GS5 831
            #define NOTE_A5  880
            #define NOTE_AS5 932
            #define NOTE_B5  988
            #define NOTE_C6  1047
            #define NOTE_CS6 1109
            #define NOTE_D6  1175
            #define NOTE_DS6 1245
            #define NOTE_E6  1319
            #define NOTE_F6  1397
            #define NOTE_FS6 1480
            #define NOTE_G6  1568
            #define NOTE_GS6 1661
            #define NOTE_A6  1760
            #define NOTE_AS6 1865
            #define NOTE_B6  1976
            #define NOTE_C7  2093
            #define NOTE_CS7 2217
            #define NOTE_D7  2349
            #define NOTE_DS7 2489
            #define NOTE_E7  2637
            #define NOTE_F7  2794
            #define NOTE_FS7 2960
            #define NOTE_G7  3136
            #define NOTE_GS7 3322
            #define NOTE_A7  3520
            #define NOTE_AS7 3729
            #define NOTE_B7  3951
            #define NOTE_C8  4186
            #define NOTE_CS8 4435
            #define NOTE_D8  4699
            #define NOTE_DS8 4978
            #define REST      0

            int tempo = 2;

            int melody[] = {

                // Gren Hill Zone - Sonic the Hedgehog
                // Score available at https://musescore.com/user/248346/scores/461661
                // Theme by Masato Nakamura, arranged by Teddy Mason
                
                REST,2, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //1
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
                REST,8, NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
                REST,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 

                REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_G4,4, NOTE_B4,8, //7
                NOTE_A4,4, NOTE_B4,8, NOTE_A4,4, NOTE_D4,2,
                REST,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
                REST,8, NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 

                REST,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //13
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
                REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_G4,4, NOTE_B4,8,
                NOTE_A4,4, NOTE_B4,8, NOTE_A4,4, NOTE_D4,8, NOTE_D4,8, NOTE_FS4,8,
                NOTE_E4,-1,
                REST,8, NOTE_D4,8, NOTE_E4,8, NOTE_FS4,-1,

                REST,8, NOTE_D4,8, NOTE_D4,8, NOTE_FS4,8, NOTE_F4,-1, //20
                REST,8, NOTE_D4,8, NOTE_F4,8, NOTE_E4,-1, //end 1

                //repeats from 1

                REST,2, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //1
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
                REST,8, NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
                REST,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 

                REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_G4,4, NOTE_B4,8, //7
                NOTE_A4,4, NOTE_B4,8, NOTE_A4,4, NOTE_D4,2,
                REST,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
                REST,8, NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 

                REST,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //13
                NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
                REST,8, NOTE_B4,8, NOTE_B4,8, NOTE_G4,4, NOTE_B4,8,
                NOTE_A4,4, NOTE_B4,8, NOTE_A4,4, NOTE_D4,8, NOTE_D4,8, NOTE_FS4,8,
                NOTE_E4,-1,
                REST,8, NOTE_D4,8, NOTE_E4,8, NOTE_FS4,-1,

                REST,8, NOTE_D4,8, NOTE_D4,8, NOTE_FS4,8, NOTE_F4,-1, //20
                REST,8, NOTE_D4,8, NOTE_F4,8, NOTE_E4,8, //end 2
                NOTE_E4,-2, NOTE_A4,8, NOTE_CS5,8, 
                NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8, NOTE_A5,-4,

            };
        
            int notes = sizeof(melody) / sizeof(melody[0]) / 2;

            // this calculates the duration of a whole note in ms
            int wholenote = (60000 * 4) / tempo;

            int divider = 0, noteDuration = 0;

            for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

                // calculates the duration of each note
                divider = melody[thisNote + 1];
                if (divider > 0) {
                // regular note, just proceed
                noteDuration = (wholenote) / divider;
                } else if (divider < 0) {
                // dotted notes are represented with negative durations!!
                noteDuration = (wholenote) / abs(divider);
                noteDuration *= 1.5; // increases the duration in half for dotted notes
                }

                // we only play the note for 90% of the duration, leaving 10% as a pause
                _tone(melody[thisNote], noteDuration * 0.9);

            }
        
        }
        
        void _loadSystem() {
            // FILE* file = fopen("../data/old.bin", "rb");
            FILE* file = fopen(OS_FILE, "rb");

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

        void _tone(double freq, int duration) {

            for (int i = 0; i < duration; i++) {
                // samples.push_back(SineWave(i, freq , 0.8));
                samples.push_back(SquareWave(i, freq , 0.8));
            }

            sndBuffer.loadFromSamples(&samples[0], samples.size(), 1, 44100);
            sound.setBuffer(sndBuffer);
            sound.play();

            while (sound.getStatus() == sound.Playing);

            samples.clear();
            sound.resetBuffer();
        }

        void _tick() {


            if (_cpu.getProgramCounter() == 0x0447) {
                // _isScnaning04Port = true;

                uint8_t d = _cpu.getRegister(Cpu::Register::D);
                uint8_t b = 0xFF - _cpu.getRegister(Cpu::Register::B);
                
                // printf("freq = %02x; duration = %02x;\n", b, d);
                _tone(b*2, d * 120);
            }

            // if (_cpu.getProgramCounter() == 0x0516) {
            //     _isScnaning04Port = true;

            //     uint8_t d = _cpu.getRegister(Cpu::Register::D);
            //     _tone(d*3, 400);
            // }

            if (_cpu.getProgramCounter() == 0x0506) {
                uint16_t delay = _cpu.getRegister(Cpu::Register::B);
                delay = (delay << 8) | _cpu.getRegister(Cpu::Register::C);
                
                for (int i = 0; i < 0xF000 * delay; i++);
            }

            _cpu.tick();
            _display.update();
            _keyboard.update();
        }


         
        void _cpuWork() {
            while (true) {
                
                if (_isCpuRunning) {
                    _tick();
                    

                    // Best delay ever
                    for (int i = 0; i < 0x300; i++);
                }
                
                // std::this_thread::sleep_for(std::chrono::microseconds(700));
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

