#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#define KB 1024


class Memory
{
public:
    Memory();
    Memory(unsigned int size);
    ~Memory();

    void reset(void);
    uint8_t fetch(uint16_t address);
    uint8_t store(uint16_t address, uint8_t value);

    std::vector<uint8_t> *getData();

protected:
    std::vector<uint8_t> data;
};
