#pragma once
#include <stdint.h>
#include "mapper.h"


namespace Mapper_0 {
    uint8_t fetchCPU(Mapper &mapper, uint16_t address);
    uint8_t fetchPPU(Mapper &mapper, uint16_t address);
    uint8_t storeCPU(Mapper &mapper, uint16_t address, uint8_t value);
    uint8_t storePPU(Mapper &mapper, uint16_t address, uint8_t value);
}
