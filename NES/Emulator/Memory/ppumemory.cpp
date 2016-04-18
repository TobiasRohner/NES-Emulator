#include "ppumemory.h"

PPUMemory::PPUMemory() : Memory(16384)
{
}


PPUMemory::~PPUMemory()
{
}

/*
Address Size 	Description
$0000 	$1000 	Pattern Table 0
$1000 	$1000 	Pattern Table 1
$2000 	$3C0 	Name Table 0
$23C0 	$40 	Attribute Table 0
$2400 	$3C0 	Name Table 1
$27C0 	$40 	Attribute Table 1
$2800 	$3C0 	Name Table 2
$2BC0 	$40 	Attribute Table 2
$2C00 	$3C0 	Name Table 3
$2FC0 	$40 	Attribute Table 3
$3000 	$F00 	Mirror of 2000h-2EFFh
$3F00 	$10 	BG Palette
$3F10 	$10 	Sprite Palette
$3F20 	$E0 	Mirror of 3F00h-3F1Fh
*/
uint8_t PPUMemory::store(uint16_t address, uint8_t value)
{
    uint8_t result = 0;
    if (address >= 0x2000 && address <= 0x2EFF) {
        Memory::store(address + 0x0000, value);
        Memory::store(address + 0x1000, value);
    }
    if (address >= 0x3000 && address <= 0x3EFF) {
        Memory::store(address - 0x0000, value);
        Memory::store(address - 0x1000, value);
    }
    if (address >= 0x3F00 && address <= 0x3F1F) {
        Memory::store(address + 0x0000, value);
        Memory::store(address + 0x0020, value);
        Memory::store(address + 0x0040, value);
        Memory::store(address + 0x0060, value);
        Memory::store(address + 0x0080, value);
        Memory::store(address + 0x00A0, value);
        Memory::store(address + 0x00C0, value);
        Memory::store(address + 0x00E0, value);
    }
    if (address >= 0x3F20 && address <= 0x3FFF) {
        uint16_t baseAddress = (address - 0x3F20) % 0x0020;
        Memory::store(0x3F00 + baseAddress, value);
        for (uint16_t a = 0x3F20 + baseAddress; a < 0x4000; a += 0x0020) {
            Memory::store(a, value);
        }
    }
    else {
        result = Memory::store(address, value);
    }
    return result;
}
