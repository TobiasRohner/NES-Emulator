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

    Debugger debugger = {};

    GameCartridge cartridge;
    std::shared_ptr<Mapper> mapper;
    std::shared_ptr<CPU_6502> cpu;
    std::shared_ptr<COMMUNCATION_BRIDGE> bridge;

    std::thread threadCPU;

    bool breakExecution = false;

    bool cpuRunning = false;

    void connectActions();

    void runCPU(std::shared_ptr<CPU_6502> cpu, bool &brk);
    void stopCPU();

private slots:
/*---------------File----------------*/
    void open();
/*---------------Run-----------------*/
    void startup();
    void execute();
/*---------------Debug---------------*/
    void openDebugger();
};

#endif // MAINWINDOW_H
