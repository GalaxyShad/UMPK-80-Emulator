#pragma once

#include "int-defines.hpp"

// #include <cstring>

// #include <exception>
// #include <iostream>

#include <cstring>

typedef uint8_t (*portInFunc_t)();
typedef void (*portOutFunc_t)(uint8_t);

#define ROM_SIZE        0x0800
#define MEMORY_SIZE     0x1000
#define PORTS_COUNT     256

class BusDeviceReadable {
    public:
        virtual uint8_t busPortRead() = 0;
};

class BusDeviceWritable {
    public:
        virtual void busPortWrite(uint8_t data) = 0;
};

class Bus {
    public:
        void memoryWrite(uint16_t adr, uint8_t data) {
            if ((adr < ROM_SIZE) || (adr >= MEMORY_SIZE)) return;

            _memory[adr] = data;
        }

        uint8_t memoryRead(uint16_t adr) {
            if (adr >= MEMORY_SIZE) {
                // printf("%04x\n", adr);
                // throw std::logic_error(std::string("Cannot read memory from " + std::to_string(adr)).c_str());
            }
                

            return _memory[adr];
        }

        void loadRom(const uint8_t* buff, size_t size) {
            memcpy(_memory, buff, size);
        }

        void loadRam(const uint8_t* buff, size_t size) {
            memcpy(_memory + ROM_SIZE, buff, size);
        }

        void portBindOut(uint8_t port, BusDeviceWritable& device) {
            _outDevices[port] = &device;
        }

        void portOut(uint8_t port, uint8_t data) {
            if (!_outDevices[port]) return;

            _outDevices[port]->busPortWrite(data);
        }

        void portBindIn(uint8_t port, BusDeviceReadable& device) {
            _inDevices[port] = &device;
        }

        uint8_t portIn(uint8_t port) {
            // return 0x00;
            return (_inDevices[port] != nullptr) ? _inDevices[port]->busPortRead() : 0x00;
        }

    private:
        uint8_t _memory[MEMORY_SIZE]  = {0};

        BusDeviceWritable*  _outDevices[PORTS_COUNT] = { nullptr };
        BusDeviceReadable*  _inDevices[PORTS_COUNT]  = { nullptr };
};