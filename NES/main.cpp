#include "mainwindow.h"
#include <QApplication>
#include "mainwindow.h"


/*
std::vector<uint8_t> openBytes(const char* path) {
    std::ifstream file(path, std::ios::binary);

    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
}

void runCPU(std::shared_ptr<CPU_6502> cpu, bool &brk) {
    cpu.get()->initialize();
    for (;;) {
        if (brk) { break; }
        cpu.get()->next();
    }
}
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
/*
    GameCartridge cart = LoadROM("E:/Programmieren/C++/NES_v2/test2.nes");
    std::shared_ptr<Mapper> mapper(new Mapper(cart));
    std::shared_ptr<COMMUNCATION_BRIDGE> bridge(new COMMUNCATION_BRIDGE);

    std::shared_ptr<CPU_6502> cpu(new CPU_6502(mapper, bridge));

    Debugger d(mapper, cpu);
    d.show();

    bool brk = false;
    std::thread threadCPU(runCPU, cpu, std::reference_wrapper<bool>(brk));
    d.updateAllRegisterDisplays();

    int returnValue = a.exec();

    brk = true;
    terminateOpcode(bridge);
    threadCPU.join();

    return returnValue;
*/
    MainWindow w(0);
    w.show();

    return a.exec();
}
