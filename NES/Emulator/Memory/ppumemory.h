#pragma once
#include "memory.h"

class PPUMemory : public Memory
{
public:
    PPUMemory();
    ~PPUMemory();

    uint8_t store(uint16_t address, uint8_t value);
};

