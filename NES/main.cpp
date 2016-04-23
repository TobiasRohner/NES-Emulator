#include "mainwindow.h"
#include <QApplication>
#include <fstream>
#include <stdint.h>
#include "../HexEditor/hexedit.h"
#include "Emulator/gamecartridge.h"
#include "Emulator/debugger.h"
#include "Emulator/cpu_6502.h"
#include "Emulator/clock.h"
#include "Emulator/Memory/Mappers/mapper.h"
#include <thread>
#include <functional>


std::vector<uint8_t> openBytes(const char* path) {
    std::ifstream file(path, std::ios::binary);

    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
}

void runCPU(CPU_6502 &cpu, bool &brk) {
    for (;;) {
        if (brk) { break; }
        cpu.next();
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameCartridge cart = LoadROM("E:/Programmieren/C++/NES_v2/test.nes");
    std::shared_ptr<Mapper> mapper(new Mapper(cart));
    std::shared_ptr<COMMUNCATION_BRIDGE> bridge(new COMMUNCATION_BRIDGE);

    CPU_6502 cpu(mapper, bridge);

    Debugger d(mapper, bridge, &cpu);
    d.show();

    bool brk = false;
    std::thread threadCPU(runCPU, std::reference_wrapper<CPU_6502>(cpu), std::reference_wrapper<bool>(brk));

    int returnValue = a.exec();

    brk = true;
    terminateOpcode(bridge);
    threadCPU.join();

    return returnValue;
}
