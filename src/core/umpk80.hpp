#pragma once

#include "bus.hpp"
#include "cpu.hpp"
#include "display.hpp"
#include "keyboard.hpp"
#include "register.hpp"

#define UMPK80_OS_SIZE 0x800

class RegisterControlStep : public BusDeviceWritable {
public:
    void busPortWrite(uint8_t data) { turnOnStepExec(); }

    bool isStepExec() { return _isStepExec; }

    void turnOnStepExec() { _isStepExec = true; }
    void turnOffStepExec() { _isStepExec = false; }

private:
    bool _isStepExec = false;
};

class Umpk80 {
public:
    Umpk80()
        : _intel8080(_bus), _keyboard(_registerScan), _registerScan(_display) {
        _bindDevices();
    }

    void port5InSet(uint8_t data) { _register5In.busPortWrite(data); }
    uint8_t port5InGet() { return _register5In.busPortRead(); }

    uint8_t port5OutGet() { return _register5Out.busPortRead(); }

    void tick() {
        if (!_registerStepExec.isStepExec())
            _intel8080.tick();
        else {
            _intel8080.tick(); // 0bd7 NOP
            _intel8080.tick(); // 0bd8 JMP USER
            _intel8080.tick(); // USER INST
            _intel8080.interruptRst(1);

            _registerStepExec.turnOffStepExec();
        }
    }

    void stop() { _intel8080.interruptRst(1); }
    void restart() { _intel8080.interruptRst(0); }

    void pressKey(KeyboardKey key) {
        switch (key) {
        case KeyboardKey::ST:
            stop();
            break;
        case KeyboardKey::R:
            restart();
            break;
        default:
            _keyboard.keyPress(key);
            break;
        }
    }

    void releaseKey(KeyboardKey key) {
        switch (key) {
        case KeyboardKey::ST:
            return;
            break;
        case KeyboardKey::R:
            return;
            break;
        default:
            _keyboard.keyRelease(key);
            break;
        }
    }

    uint8_t getDisplayDigit(int digit) { return _display.get(digit); }

    void loadOS(const uint8_t *os) { _bus.loadRom(os, UMPK80_OS_SIZE); }

    Cpu &getCpu() { return _intel8080; }
    Bus &getBus() { return _bus; }

private:
    Cpu _intel8080;
    Bus _bus;

    // Devices
    Keyboard _keyboard;
    Display _display;
    RegisterScanDevice _registerScan;
    RegisterDevice _register5In;
    RegisterDevice _register5Out;

    RegisterControlStep _registerStepExec;

private:
#ifdef EMULATE_OLD_UMPK
    const uint8_t PORT_SPEAKER = 0x04;
    const uint8_t PORT_IO = 0x05;
    const uint8_t PORT_KEYBOARD = 0x18;
    const uint8_t PORT_DISPLAY = 0x38;
    const uint8_t PORT_SCAN = 0x28;
#else
    const uint8_t PORT_SPEAKER  = 0x04;
    const uint8_t PORT_IO       = 0x05;
    const uint8_t PORT_KEYBOARD = 0x06;
    const uint8_t PORT_DISPLAY  = 0x06;
    const uint8_t PORT_SCAN     = 0x07;
#endif
private:
    void _bindDevices() {
        _bus.portBindOut(PORT_SCAN, _registerScan);

        _bus.portBindIn(PORT_KEYBOARD, _keyboard);
        _bus.portBindOut(PORT_DISPLAY, _display);

        _bus.portBindIn(PORT_IO, _register5In);
        _bus.portBindOut(PORT_IO, _register5Out);

        _bus.portBindOut(0xE, _registerStepExec);
    }
};