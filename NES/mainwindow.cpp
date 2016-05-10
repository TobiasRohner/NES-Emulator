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

    bridge = std::shared_ptr<COMMUNCATION_BRIDGE>(new COMMUNCATION_BRIDGE);
    cpu = std::shared_ptr<CPU_6502>(new CPU_6502(mapper, bridge));
}

MainWindow::~MainWindow()
{
    stopCPU();
    delete ui;
}

void MainWindow::loadCartridge(std::string path)
{
    cartridge = LoadROM(path);
    mapper = std::shared_ptr<Mapper>(new Mapper(cartridge));
    cpu = std::shared_ptr<CPU_6502>(new CPU_6502(mapper, bridge));
}

bool MainWindow::getCPURunning()
{
    return cpuRunning;
}

void MainWindow::connectActions()
{
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionOpenDebugger, &QAction::triggered, this, &MainWindow::openDebugger);
    connect(ui->actionStartup, &QAction::triggered, this, &MainWindow::startup);
    connect(ui->actionExecute, &QAction::triggered, this, &MainWindow::execute);
}

void MainWindow::runCPU(std::shared_ptr<CPU_6502> cpu, bool &brk)
{
    cpu.get()->initialize();
    for (;;) {
        if (brk) { break; }
        cpu.get()->next();
    }
}

void MainWindow::stopCPU()
{
    breakExecution = true;
    terminateOpcode(bridge);
    threadCPU.join();
    breakExecution = false;
}

void MainWindow::open()
{
    QString file = QFileDialog::getOpenFileName(this, "Open a ROM", QDir::currentPath(), "*.nes");
    loadCartridge(file.toStdString());
}

void MainWindow::startup()
{
    threadCPU = std::thread(runCPU, this, cpu, std::reference_wrapper<bool>(breakExecution));
}

void MainWindow::execute()
{
    //TODO
}

void MainWindow::openDebugger()
{
    debugger.setData(mapper, cpu);
    debugger.show();
}

