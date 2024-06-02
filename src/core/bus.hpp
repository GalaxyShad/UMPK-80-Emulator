#pragma once

#include "inttypes.hpp"

typedef u8 (*portInFunc_t)();
typedef void (*portOutFunc_t)(u8);

#define ROM_SIZE        0x0800
#define MEMORY_SIZE     0x1000
#define PORTS_COUNT     256

class BusDeviceReadable {
    public:
        virtual u8 busPortRead() = 0;
};

class BusDeviceWritable {
    public:
        virtual void busPortWrite(u8 data) = 0;
};

class Bus {
    public:
        void memoryWrite(u16 adr, u8 data) {
            if (adr < ROM_SIZE) return;

            _memory[adr & 0x0FFF] = data;
        }

        u8 memoryRead(u16 adr) {
            return _memory[adr & 0x0FFF];
        }

        const u8& romFirst() { return _memory[0]; }
        const u8& ramFirst() { return _memory[0x0800]; }

        void loadRom(const u8* buff, u64 size) {
            for (u64 i = 0; i < size; _memory[i] = buff[i], ++i);
        }

        void loadRam(const u8* buff, u64 size, u64 ramShift = 0) {
            for (u64 i = 0; i < size; _memory[i + ROM_SIZE + ramShift] = buff[i], ++i);
        }

        void portBindOut(u8 port, BusDeviceWritable& device) {
            _outDevices[port] = &device;
        }

        void portOut(u8 port, u8 data) {
            if (!_outDevices[port]) return;

            _outDevices[port]->busPortWrite(data);
        }

        void portBindIn(u8 port, BusDeviceReadable& device) {
            _inDevices[port] = &device;
        }

        u8 portIn(u8 port) {
            return (_inDevices[port] != nullptr) ? _inDevices[port]->busPortRead() : 0x00;
        }

    private:
        u8 _memory[MEMORY_SIZE]  = {0};

        BusDeviceWritable*  _outDevices[PORTS_COUNT] = { nullptr };
        BusDeviceReadable*  _inDevices[PORTS_COUNT]  = { nullptr };
};