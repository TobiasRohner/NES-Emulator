#pragma once
#include <stdint.h>
#include "../cpumemory.h"
#include "../ppumemory.h"
#include "../../gamecartridge.h"
#include <mutex>


class Mapper
{
public:
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

    CPUMemory memoryCPU = CPUMemory();
    PPUMemory memoryPPU = PPUMemory();

private:
    std::mutex lock;
};
