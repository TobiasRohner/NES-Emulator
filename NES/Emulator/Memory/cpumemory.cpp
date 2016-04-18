#include "cpumemory.h"

CPUMemory::CPUMemory() : Memory(65536)//Memory(65536)
{
}


CPUMemory::~CPUMemory()
{
}

/*
-------------------- CPU Memory Map --------------------
Address	Size	Description
$0000 	$800 	2KB of work RAM
$0800 	$800 	Mirror of $000-$7FF
$1000 	$800 	Mirror of $000-$7FF
$1800 	$800 	Mirror of $000-$7FF
$2000 	8 		PPU Ctrl Registers
$2008 	$1FF8 	Mirror of $2000-$2007
$4000 	$20 	Registers (Mostly APU)
$4020 	$1FDF 	Cartridge Expansion ROM
$6000 	$2000 	SRAM
$8000 	$4000 	PRG-ROM
$C000 	$4000 	PRG-ROM
*/
uint8_t CPUMemory::store(uint16_t address, uint8_t value)
{
    uint8_t result = 0;
    if ((address >= 0x0000 && address <= 0x07FF) ||									/*Work RAM*/
        (address >= 0x0800 && address <= 0x0FFF) ||
        (address >= 0x1000 && address <= 0x17FF) ||
        (address >= 0x1800 && address <= 0x1FFF)) {
        uint16_t baseAddress = address % 0x0800;
        result = Memory::store(baseAddress + 0x0000, value);
        result = Memory::store(baseAddress + 0x0800, value);
        result = Memory::store(baseAddress + 0x1000, value);
        result = Memory::store(baseAddress + 0x1800, value);
    }
    if (address >= 0x2000 && address <= 0x3FFF) {									/*PPU Control Registers*/
        uint16_t baseAddress = (address - 0x2000) % 0x0008;
        for (uint16_t a = 0x2000 + baseAddress; a < 0x4000; a += 0x0008) {
            result = Memory::store(a, value);
        }
    }
    else {																			/*Non-Mirrored Addresses*/
        result = Memory::store(address, value);
    }
    return result;
}
