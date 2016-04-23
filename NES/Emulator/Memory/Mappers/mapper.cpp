#include "mapper.h"
#include "mapper_0.h"
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

GameCartridge LoadROM(std::string path)
{
    std::ifstream f;
    f.open(path, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(f)),
        (std::istreambuf_iterator<char>()));

    if (content[0] == 0x4E && content[1] == 0x45 && content[2] == 0x53 && content[3] == 0x1A) {
        /*Load the file header*/
        /*
        Starting Byte	Length (Bytes)	Contents
        0				3				"NES" file identifier
        3				1				0x1A file identifier
        4				1				Number of 16KB PRG-ROM Banks
        5				1				Number of 8KB CHR-ROM / VROM Banks
        6				1				ROM Control Byte 1
        Bit 0:		Indicates type of mirroring (0: horizontal mirroring, 1: vertical mirroring)
        Bit 1:		Indicates the presence of a battery-backed RAM at 0x6000-0x7FFF
        Bit 2:		Indicates the presence of a 512-byte trainer at 0x7000-0x71FF
        Bit 3:		If this bit is 1, use four screen mirroring (if set overwrites Bit 0)
        Bits 4-7:	Four lower bits of the mapper number
        7				1				ROM Control Byte 2
        Bits 0-3:	Reserved for future usage
        Bits 4-7:	Four lower Bits of the mapper number
        8				1				Number of 8KB RAM Banks (Assume 1 if this byte is 0x00 due to compatibility reasons)
        9				7				Reserved for future use
        */
        GameCartridge cartridge;

        cartridge.properties.prgRomBankCount = (uint8_t)content[4];
        cartridge.properties.chrRomBankCount = (uint8_t)content[5];
        cartridge.properties.ramBankCount = (uint8_t)std::min((int)content[8], 1);
        uint8_t ctrlByte1 = (uint8_t)content[6];
        uint8_t ctrlByte2 = (uint8_t)content[7];
        cartridge.properties.batteryBackedRam = (ctrlByte1 >> 0x01) & 0x01 != 0;
        cartridge.properties.trainerPresent = (ctrlByte1 >> 0x02) & 0x01 != 0;
        cartridge.properties.mapperNr = ((ctrlByte2 & 0xF0) >> 4) | (ctrlByte1 & 0x0F);
        bool mirroringType = ctrlByte1 & 0x01 != 0;
        bool fourScreenMirroring = (ctrlByte1 >> 0x03) & 0x01 != 0;
        cartridge.properties.mirroringType = mirroringType ? 1 : 0;
        cartridge.properties.mirroringType = fourScreenMirroring ? 2 : cartridge.properties.mirroringType;

        /*Load the 512-byte trainer if one is present*/
        if ((ctrlByte1 >> 1) & 0x01 != 0x00) {														/*512-byte trainer at 0x7000-0x71FF is present*/
            cartridge.trainer = Memory(512);																	/*Initialize the trainer Memory and store the data read from the file to it*/
            for (unsigned int address = 0; address < 512; address++) {
                cartridge.trainer.store((uint16_t)address, (uint8_t)content[16 + address]);
            }
        }

        /*Load all PRG-ROM's*/
        cartridge.prgBanks = Memory(cartridge.properties.prgRomBankCount * 16384);
        for (unsigned int currentPrgBankIdx = 0; currentPrgBankIdx < cartridge.properties.prgRomBankCount * 16384; currentPrgBankIdx++) {
            uint16_t currentPrgBankMemIdx = (cartridge.properties.trainerPresent ? 16 + 512 : 16) + currentPrgBankIdx;
            cartridge.prgBanks.store(currentPrgBankIdx, content[currentPrgBankMemIdx]);
        }

        /*Load all CHR-ROM's*/
        cartridge.chrBanks = Memory(cartridge.properties.chrRomBankCount * 8192);
        for (unsigned int currentChrBankIdx = 0; currentChrBankIdx < cartridge.properties.chrRomBankCount * 8192; currentChrBankIdx++) {
            uint16_t currentChrBankMemIdx = (cartridge.properties.trainerPresent ? 16 + 512 : 16) + (cartridge.properties.prgRomBankCount * 16384) + currentChrBankIdx;
            cartridge.chrBanks.store(currentChrBankIdx, content[currentChrBankMemIdx]);
        }

        return cartridge;
    }
}
