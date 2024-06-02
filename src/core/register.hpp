#pragma once

#include "bus.hpp"
#include "display.hpp"

class RegisterScanDevice : public BusDeviceReadable, public BusDeviceWritable {
    public:
        RegisterScanDevice(Display& disp) : _disp(disp) {}

        u8      busPortRead()          { return  _data; }
        void    busPortWrite(u8 data)  {  _data = data; _disp.lightup(_data); }
    private:
        u8 _data = 0xFF;
        Display& _disp;

};

class RegisterDevice : public BusDeviceReadable, public BusDeviceWritable {
    public:
        u8      busPortRead()          { return  _data; }
        void    busPortWrite(u8 data)  {  _data = data; }
    private:
        u8 _data = 0xFF;
};