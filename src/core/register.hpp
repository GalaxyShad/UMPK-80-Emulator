#pragma once

#include "bus.hpp"
#include "display.hpp"

class RegisterScanDevice : public BusDeviceReadable, public BusDeviceWritable {
    public:
        RegisterScanDevice(Display& disp) : _disp(disp) {}

        uint8_t busPortRead()               { return  _data; }
        void    busPortWrite(uint8_t data)  {  _data = data; _disp.lightup(_data); }
    private:
        uint8_t _data = 0xFF;
        Display& _disp;

};

class RegisterDevice : public BusDeviceReadable, public BusDeviceWritable {
    public:
        uint8_t busPortRead()               { return  _data; }
        void    busPortWrite(uint8_t data)  {  _data = data; }
    private:
        uint8_t _data = 0xFF;
};

class RegisterSoundDevice: public BusDeviceWritable {
public:
    void busPortWrite(uint8_t data) override {
        buffer[shift] = (data != 0x00);
        shift++;

        if (shift == 0) {
            for (int i = 0; i < 256; i++)
                buffer[i] = 0;
        }
    }

    uint8_t getShift() { return shift; }
    size_t getSize() {  return 256; }
    const bool* getBuffer() { return buffer; }
private:
    bool buffer[256] = {0};
    uint8_t shift = 0;
};