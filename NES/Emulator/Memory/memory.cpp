#include "memory.h"
#include "options.h"
#include <iostream>

Memory::Memory() {
}

Memory::Memory(unsigned int size)
{
#ifdef DEBUG
    std::cout << "Initializing Memory with ";
    std::cout << size;
    std::cout << " elements" << std::endl;
#endif
    data = std::vector<uint8_t>(size);
    reset();
}

Memory::~Memory()
{
}

void Memory::reset(void)
/*Reset the memory to 0*/
{
    for (unsigned long e = 0; e < data.size(); ++e) {
        data[e] = 0;
    }
}

uint8_t Memory::fetch(uint16_t address)
/*Return the value at the given address in RAM*/
{
    return data[address];
}

uint8_t Memory::store(uint16_t address, uint8_t value)
/*Store a number to the specified address in memory and return the old value*/
{
    const uint8_t oldValue = data[address];
    data[address] = value;
    return oldValue;
}

std::vector<uint8_t> *Memory::getData() {
    return &data;
}
