#pragma once
#include <stdint.h>
#include "Memory/memory.h"


struct CartridgeProperties {
    uint8_t prgRomBankCount = 1;
    uint8_t chrRomBankCount = 1;
    uint8_t ramBankCount = 1;
    uint8_t mirroringType = 0;			/*0: horizontal mirroring, 1: vertical mirroring, 2:four screen mirroring*/
    uint8_t mapperNr = 0;
    bool batteryBackedRam = false;
    bool trainerPresent = false;
};

struct GameCartridge {
    Memory prgBanks = Memory(16384);
    Memory chrBanks = Memory(8192);
    Memory ram = Memory(8192);
    Memory trainer = Memory(512);
    CartridgeProperties properties;
};
