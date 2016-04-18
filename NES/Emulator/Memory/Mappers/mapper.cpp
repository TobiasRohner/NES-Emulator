#include "mapper.h"
#include "Mapper_0.h"
#include <iostream>
#include <fstream>
#include <algorithm>



Mapper::Mapper(GameCartridge cartridge)
{
    Mapper::cartridge = cartridge;
    prgBankPointer1 = 0;
    prgBankPointer2 = cartridge.properties.prgRomBankCount - 1;
    chrBankPointer = 0;
}

Mapper::~Mapper()
{
}

uint8_t Mapper::fetchCPU(uint16_t address)
{
    std::lock_guard<std::mutex> guard(lock);
    switch (cartridge.properties.mapperNr) {
    case 0:
        return Mapper_0::fetchCPU(*this, address);
    }
}

uint8_t Mapper::fetchPPU(uint16_t address)
{
    std::lock_guard<std::mutex> guard(lock);
    switch (cartridge.properties.mapperNr) {
    case 0:
        return Mapper_0::fetchPPU(*this, address);
    }
}

uint8_t Mapper::storeCPU(uint16_t address, uint8_t value)
{
    std::lock_guard<std::mutex> guard(lock);
    switch (cartridge.properties.mapperNr) {
    case 0:
        return Mapper_0::storeCPU(*this, address, value);
    }
}

uint8_t Mapper::storePPU(uint16_t address, uint8_t value)
{
    std::lock_guard<std::mutex> guard(lock);
    switch (cartridge.properties.mapperNr) {
    case 0:
        return Mapper_0::storePPU(*this, address, value);
    }
}

void Mapper::cpu_to_file(std::string path)
{
    std::lock_guard<std::mutex> guard(lock);
    std::ofstream f;
    f.open(path);
    for (unsigned long address = 0; address < 64 * 1024; address++) {
        f << fetchCPU(address);
    }
    f.close();
}

void Mapper::all_to_file(std::string path)
{
    std::lock_guard<std::mutex> guard(lock);
    std::ofstream f;
    f.open(path);
    f << "Memory CPU:" << std::endl;
    for (unsigned long address = 0; address < 65536; address++) {
        f << memoryCPU.fetch(address);
    }
    f << std::endl;
    for (int prgBank = 0; prgBank < cartridge.properties.prgRomBankCount; prgBank++) {
        f << "PRG-ROM-" << std::to_string(prgBank) << std::endl;
        for (unsigned long address = 16 * 1024 * prgBank; address < 16 * 1024 * (prgBank + 1); address++) {
            f << cartridge.prgBanks.fetch(address);
        }
    }
    for (int chrBank = 0; chrBank < cartridge.properties.chrRomBankCount; chrBank++) {
        f << "CHR-ROM-" << std::to_string(chrBank) << std::endl;
        for (unsigned long address = 8 * 1024 * chrBank; address < 8 * 1024 * (chrBank + 1); address++) {
            f << cartridge.chrBanks.fetch(address);
        }
    }
    for (int ramBank = 0; ramBank < cartridge.properties.chrRomBankCount; ramBank++) {
        f << "RAM-" << std::to_string(ramBank) << std::endl;
        for (unsigned long address = 8 * 1024 * ramBank; address < 8 * 1024 * (ramBank + 1); address++) {
            f << cartridge.ram.fetch(address);
        }
    }
    f.close();
}
