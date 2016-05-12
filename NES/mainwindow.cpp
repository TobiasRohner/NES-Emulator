#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <string>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectActions();

    bridgeCPU = std::shared_ptr<COMMUNCATION_BRIDGE>(new COMMUNCATION_BRIDGE);
    bridgePPU = std::shared_ptr<COMMUNCATION_BRIDGE>(new COMMUNCATION_BRIDGE);
    cpu = std::shared_ptr<CPU_6502>(new CPU_6502(mapper, bridgeCPU));
}

MainWindow::~MainWindow()
{
    stopClock();
    stopCPU();
    delete ui;
}

void MainWindow::loadCartridge(std::string path)
{
    cartridge = LoadROM(path);
    mapper = std::shared_ptr<Mapper>(new Mapper(cartridge));
    cpu = std::shared_ptr<CPU_6502>(new CPU_6502(mapper, bridgeCPU));
}

bool MainWindow::getCPURunning()
{
    return cpuRunning;
}

void MainWindow::connectActions()
{
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionOpenDebugger, &QAction::triggered, this, &MainWindow::openDebugger);
    connect(ui->actionExecute, &QAction::triggered, this, &MainWindow::execute);
    connect(ui->actionStop_Execution, &QAction::triggered, this, &MainWindow::stopExecution);
}

void MainWindow::startupCPU()
{
    threadCPU = std::thread(runCPU, this, cpu, debugger, std::reference_wrapper<bool>(stopCPUExecution));
    cpuRunning = true;
}

void MainWindow::runCPU(std::shared_ptr<CPU_6502> cpu, std::shared_ptr<Debugger> debugger, bool &brk)
{
    cpu.get()->initialize();
    while (!brk) {
        cpu.get()->next();
#ifdef DEBUG
        if (debugger.get()->isVisible()) {
            debugger.get()->updateHexEditCPU();
        }
#endif
    }
    brk = false;
}

void MainWindow::stopCPU()
{
    if (cpuRunning) {
        stopCPUExecution = true;
        terminateOpcode(bridgeCPU);
        while (stopCPUExecution) { }      //Find a better Way!
        threadCPU.join();
        cpuRunning = false;
    }
}

void MainWindow::startupClock()
{
    threadClock = std::thread(runClock, this, bridgeCPU, bridgePPU, std::reference_wrapper<bool>(stopClockExecution));
    clockRunning = true;
}

void MainWindow::runClock(std::shared_ptr<COMMUNCATION_BRIDGE> bridgeCPU, std::shared_ptr<COMMUNCATION_BRIDGE> bridgePPU, bool &brk)
{
    while (!brk) {
        bridgeCPU.get()->clk.notify_all();
        std::this_thread::sleep_for(std::chrono::duration<double>(MASTER_CLOCK_PERIOD * 4));     //Multiply by 4 as it is the biggest common divisor of the clock dividors with factors 4 (PPU) and 12 (CPU)
        bridgePPU.get()->clk.notify_all();
        std::this_thread::sleep_for(std::chrono::duration<double>(MASTER_CLOCK_PERIOD * 4));
        bridgePPU.get()->clk.notify_all();
        std::this_thread::sleep_for(std::chrono::duration<double>(MASTER_CLOCK_PERIOD * 4));
        bridgePPU.get()->clk.notify_all();
    }
    brk = false;
}

void MainWindow::stopClock()
{
    if (clockRunning) {
        stopClockExecution = true;
        while (stopClockExecution) { }      //Find a better Way!
        threadClock.join();
        clockRunning = false;
    }
}

void MainWindow::open()
{
    QString file = QFileDialog::getOpenFileName(this, "Open a ROM", QDir::currentPath(), "*.nes");
    loadCartridge(file.toStdString());
}

void MainWindow::execute()
{
    if (!cpuRunning) { startupCPU(); }
    startupClock();
}

void MainWindow::stopExecution()
{
    stopClock();
    stopCPU();
}

void MainWindow::openDebugger()
{
    if (!cpuRunning) { startupCPU(); }
    debugger.get()->setData(mapper, cpu);
    debugger.get()->show();
}

