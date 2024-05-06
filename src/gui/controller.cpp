#include "controller.hpp"
#include <fstream>
#include <iomanip>

void Controller::decompileToFile(std::string filename, uint16_t fromAdr,
                                 uint16_t len) {
    std::ofstream stream(filename);

    Disassembler dis(getRom() + fromAdr, len);

    dis.reset();

    for (int i = 0; i < len; i++) {
        auto disres = dis.disassemble();

        auto mc = std::vector<uint8_t>(disres.bytes, disres.bytes + disres.bytesCount);
        uint16_t adr = dis.getPgCounter();

        std::string instr = dis.disassemble().toString();

        stream << std::hex << std::uppercase << std::setw(4)
               << std::setfill('0') << adr << " | ";

        for (int j = 0; j < 3; j++) {
            if (j == 1)
                stream << ' ';

            if (j < mc.size()) {
                stream << std::hex << std::uppercase << std::setw(2)
                       << std::setfill('0') << (int)mc[j];
            } else {
                stream << "  ";
            }
        }

        stream << " | " << instr << '\n';
    }

    stream.close();
}

void Controller::onBtnStart() {
    _umpkMutex.lock();
    _isUmpkFreezed = false;
    _umpkMutex.unlock();
}

void Controller::onButtonStop() {
    _umpkMutex.lock();
    _isUmpkFreezed = true;
    _umpkMutex.unlock();
}

void Controller::onBtnNextCommand() {
    _umpkMutex.lock();
    _umpk.tick();
    _umpkMutex.unlock();
}

void Controller::onBtnReset() {
    _umpkMutex.lock();
    _umpk.restart();
    _umpkMutex.unlock();
}

void Controller::setUmpkKey(KeyboardKey key, bool value) {
    _umpkMutex.lock();
    (value == true) ? _umpk.pressKey(key) : _umpk.releaseKey(key);
    _umpkMutex.unlock();
}

void Controller::port5In(uint8_t data) {
    _umpkMutex.lock();
    _umpk.port5InSet(data);
    _umpkMutex.unlock();
}

void Controller::setCpuFlags(CpuFlagsMapping flags) {
    _umpkMutex.lock();
    _umpk.getCpu().setFlags(flags);
    _umpkMutex.unlock();
}

void Controller::setCpuProgramCounter(uint16_t value) {
    _umpkMutex.lock();
    _umpk.getCpu().setProgramCounter(value);
    _umpkMutex.unlock();
}

void Controller::setCpuStackPointer(uint16_t sp) {
    _umpkMutex.lock();
    _umpk.getCpu().setStackPointer(sp);
    _umpkMutex.unlock();
}

void Controller::setMemory(uint16_t index, uint8_t data) {
    _umpkMutex.lock();
    _umpk.getBus().memoryWrite(index, data);
    _umpkMutex.unlock();
}

std::vector<uint8_t> Controller::readBinaryFile(std::string path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file " + path);
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(fileSize);
    if (!file.read(reinterpret_cast<char *>(data.data()), fileSize)) {
        throw std::runtime_error("Failed to read file " + path);
    }
    file.close();

    return data;
}

void Controller::loadProgramToMemory(uint16_t position,
                                     std::vector<uint8_t> &program) {
    _umpkMutex.lock();
    for (size_t i = 0; i < program.size(); i++) {
        _umpk.getBus().memoryWrite(position + i, program[i]);
    }
    _umpkMutex.unlock();
}

void Controller::_loadSystem() {
    char os[0x800] = {0};

    std::ifstream file(OS_FILE, std::ios::binary);

    file.read(os, 0x800);
    file.close();

    _umpk.loadOS((const uint8_t *)os);
}

void Controller::_handleHooks(Cpu &cpu) {
    uint16_t pgCounter = cpu.getProgramCounter();

    const uint16_t SOUND_FUNC_ADR = 0x0447;
    const uint16_t DELAY_FUNC_ADR = 0x0506;
    const uint16_t START_END_ADR  = 0x00C5;

    if (pgCounter == START_END_ADR) {
        _gui.onUmpkOsStartupFinished();
    }

    if (pgCounter == SOUND_FUNC_ADR) {
        uint8_t duration = cpu.getRegister(Cpu::Register::D);
        uint8_t frequency = 0xFF - cpu.getRegister(Cpu::Register::B);

        dj.tone(frequency * 2, duration * 130);
    }

    if (pgCounter == DELAY_FUNC_ADR) {
        uint16_t delay = cpu.getRegister(Cpu::Register::B);
        delay = (delay << 8) | cpu.getRegister(Cpu::Register::C);

        for (int i = 0; i < 0xF000 * delay; i++)
            ;
    }
}

void Controller::_umpkWork() {
    _umpkMutex.lock();
    _loadSystem();
    _umpkMutex.unlock();

    while (_isUmpkWorking) {
        if (_isUmpkFreezed)
            continue;

        _umpkMutex.lock();
        _umpk.tick();
        _umpkMutex.unlock();

        _handleHooks(_umpk.getCpu());

        if (breakpoint == _umpk.getCpu().getProgramCounter())
            _isUmpkFreezed = true;
    }
}

void Controller::_copyTestToMemory(uint16_t startAdr, uint8_t *test,
                                   size_t size) {
    for (size_t i = 0; i < size; i++) {
        _umpk.getBus().memoryWrite(startAdr + i, test[i]);
    }
}
