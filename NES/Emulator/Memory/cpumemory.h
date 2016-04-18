#pragma once
#include "memory.h"
#include <stdint.h>

class CPUMemory : public Memory
{
public:
    CPUMemory();
    ~CPUMemory();

    uint8_t store(uint16_t address, uint8_t value);
};
