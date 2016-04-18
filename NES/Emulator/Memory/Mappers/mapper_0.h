#include <stdint.h>
#include "mapper.h"


namespace Mapper_0 {
    uint8_t fetchCPU(Mapper &mapper, uint16_t address) {
        if (address >= 0x6000 && address <= 0x7FFF) {
                uint16_t realAddress = address - 0x6000;
                return mapper.cartridge.ram.fetch(realAddress);
            }
            if (address >= 0x8000 && address <= 0xBFFF) {
                uint16_t realAddress = (address - 0x8000) + (mapper.prgBankPointer1 * 16384);
                return mapper.cartridge.prgBanks.fetch(realAddress);
            }
            if (address >= 0xC000 && address <= 0xFFFF) {
                uint16_t realAddress = (address - 0xC000) + (mapper.prgBankPointer2 * 16384);
                return mapper.cartridge.prgBanks.fetch(realAddress);
            }
            return mapper.memoryCPU.fetch(address);
    }

    uint8_t fetchPPU(Mapper &mapper, uint16_t address) {
        return mapper.memoryPPU.fetch(address);
    }

    uint8_t storeCPU(Mapper &mapper, uint16_t address, uint8_t value) {
        if (address >= 0x6000 && address <= 0x7FFF) {
                uint16_t realAddress = address - 0x6000;
                return mapper.cartridge.ram.store(realAddress, value);
            }
            if (address >= 0x8000 && address <= 0xBFFF) {
                uint16_t realAddress = (address - 0x8000) + (mapper.prgBankPointer1 * 16384);
                return mapper.cartridge.prgBanks.store(realAddress, value);
            }
            if (address >= 0xC000 && address <= 0xFFFF) {
                uint16_t realAddress = (address - 0xC000) + (mapper.prgBankPointer2 * 16384);
                return mapper.cartridge.prgBanks.store(realAddress, value);
            }
            return mapper.memoryCPU.store(address, value);
    }

    uint8_t storePPU(Mapper &mapper, uint16_t address, uint8_t value) {
        return mapper.memoryPPU.store(address, value);
    }
}
