#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "Emulator/Memory/Mappers/mapper.h"
#include "Emulator/gamecartridge.h"
#include "Emulator/cpu_6502.h"
#include "Emulator/debugger.h"
#include "Emulator/clock.h"
#include <thread>
#include "options.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadCartridge(std::string path);

    bool getCPURunning();

    const double MASTER_CLOCK_FREQUENCY = 236250000.0/11.0;
    const double MASTER_CLOCK_PERIOD    = 1.0/MASTER_CLOCK_FREQUENCY;
    
private:
    Ui::MainWindow *ui;

    std::shared_ptr<Debugger> debugger = std::shared_ptr<Debugger>(new Debugger);

    GameCartridge cartridge;
    std::shared_ptr<Mapper> mapper;
    std::shared_ptr<CPU_6502> cpu;
    std::shared_ptr<COMMUNCATION_BRIDGE> bridgeCPU;
    std::shared_ptr<COMMUNCATION_BRIDGE> bridgePPU;

    std::thread threadCPU;
    std::thread threadClock;

    bool stopCPUExecution = false;
    bool stopClockExecution = false;

    bool cpuRunning = false;
    bool clockRunning = false;

    void connectActions();

    void startupCPU();
    void runCPU(std::shared_ptr<CPU_6502> cpu, std::shared_ptr<Debugger> debugger, bool &brk);
    void stopCPU();
    void startupClock();
    void runClock(std::shared_ptr<COMMUNCATION_BRIDGE> bridgeCPU, std::shared_ptr<COMMUNCATION_BRIDGE> bridgePPU, bool &brk);
    void stopClock();

private slots:
/*---------------File----------------*/
    void open();
/*---------------Run-----------------*/
    void execute();
    void stopExecution();
/*---------------Debug---------------*/
    void openDebugger();
};

#endif // MAINWINDOW_H
