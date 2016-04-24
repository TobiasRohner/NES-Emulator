#pragma once
#include <stdint.h>
#include "../memory.h"
#include "../../gamecartridge.h"
#include <mutex>


class Mapper
{
public:
    Mapper();
    Mapper(GameCartridge cartridge);
    ~Mapper();

    uint8_t fetchCPU(uint16_t address);
    uint8_t fetchPPU(uint16_t address);
    uint8_t storeCPU(uint16_t address, uint8_t value);
    uint8_t storePPU(uint16_t address, uint8_t value);

    void all_to_file(std::string path);
    void cpu_to_file(std::string path);

    GameCartridge cartridge;
    int prgBankPointer1 = 0;
    int prgBankPointer2 = 0;
    int chrBankPointer = 0;

    Memory memoryCPU = Memory(64*KB - 7.96875*KB - 8*KB - 2*16*KB);
    Memory memoryPPU = Memory(16*KB - 0x2000);

private:
    std::mutex lock;
};


GameCartridge LoadROM(std::string path);
