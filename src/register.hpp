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