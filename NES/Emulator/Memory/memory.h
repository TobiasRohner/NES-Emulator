#pragma once
#include <stdint.h>
#include <string>
#include <vector>


class Memory
{
public:
    Memory(unsigned int size);
    ~Memory();


    void reset(void);
    uint8_t fetch(uint16_t address);
    uint8_t store(uint16_t address, uint8_t value);

protected:
    std::vector<uint8_t> data;
};
