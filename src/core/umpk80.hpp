#pragma once

#include "bus.hpp"
#include "cpu.hpp"
#include "display.hpp"
#include "keyboard.hpp"
#include "register.hpp"

#define UMPK80_OS_SIZE 0x800

class RegisterControlStep : public BusDeviceWritable {
public:
    void busPortWrite(u8 data) { turnOnStepExec(); }

    bool isStepExec() { return _isStepExec; }

    void turnOnStepExec() { _isStepExec = true; }
    void turnOffStepExec() { _isStepExec = false; }

private:
    bool _isStepExec = false;
};

class Umpk80 {
private:
    const u16 SAVPC = 0x0BDC;
public:
    Umpk80()
        : _intel8080(_bus), _keyboard(_registerScan), _registerScan(_display) {
        _bindDevices();
    }

    enum class Register {
        PC_LOW, PC_HIGH,
        SP_LOW, SP_HIGH,
        L,   H,
        E,   D,
        C,   B,
        PSW, A,
        M
    };

    enum class RegisterPair {
        PC,
        SP,
        HL,
        DE,
        BC,
        PSWA,
    };

    void port5InSet(u8 data) { _register5In.busPortWrite(data); }
    u8 port5InGet() { return _register5In.busPortRead(); }

    u8 port5OutGet() { return _register5Out.busPortRead(); }

    void tick() {
        if (!_registerStepExec.isStepExec()) {
            _intel8080.tick();
        } else {
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
            case KeyboardKey::R:
                return;
            default:
                _keyboard.keyRelease(key);
                break;
        }
    }

    bool getKeyState(KeyboardKey key) { return _keyboard.isKeyPressed(key); }

    u8 getDisplayDigit(int digit) { return _display.get(digit); }

    void loadOS(const u8 *os) { _bus.loadRom(os, UMPK80_OS_SIZE); }

    u16 getRegisterPair(RegisterPair regPair) {
        u8 low  = _bus.memoryRead(SAVPC + (u16)regPair * 2);
        u8 high = _bus.memoryRead(SAVPC + (u16)regPair * 2 + 1);

        return ((u16)high << 8) | low;
    }

    void setRegisterPair(RegisterPair regPair, u16 value) {
        u16 adrlow  = SAVPC + (u16)regPair * 2;
        u16 adrhigh = SAVPC + (u16)regPair * 2 + 1;

        _bus.memoryWrite(adrlow,  value & 0xFF);
        _bus.memoryWrite(adrhigh, (value >> 8) & 0xFF);
    }

    u8 getRegister(Register reg) {
        if (reg == Register::M) {
            u16 hl = getRegisterPair(RegisterPair::HL);
            return _bus.memoryRead(hl);
        }

        return _bus.memoryRead(SAVPC + (u16)reg);
    }

    void setRegister(Register reg, u8 value) {
        if (reg == Register::M) {
            // TODO Handle it somehow?
            return;
        }

        _bus.memoryWrite(SAVPC + (u16)reg, value);
    }

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
public:
#ifdef EMULATE_OLD_UMPK
    const u8 PORT_SPEAKER = 0x04;
    const u8 PORT_IO = 0x05;
    const u8 PORT_KEYBOARD = 0x18;
    const u8 PORT_DISPLAY = 0x38;
    const u8 PORT_SCAN = 0x28;
#else
    const u8 PORT_SPEAKER  = 0x04;
    const u8 PORT_IO       = 0x05;
    const u8 PORT_KEYBOARD = 0x06;
    const u8 PORT_DISPLAY  = 0x06;
    const u8 PORT_SCAN     = 0x07;
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