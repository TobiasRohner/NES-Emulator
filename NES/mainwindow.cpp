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
//    connectActions();

//    bridge(new COMMUNCATION_BRIDGE);
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*
void MainWindow::loadCartridge(std::string path) {
    cartridge = LoadROM(path);
    mapper(new Mapper(cartridge));
    cpu(new CPU_6502(mapper, bridge));
}

void MainWindow::connectActions() {
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
}

void MainWindow::runCPU(std::shared_ptr<CPU_6502> cpu, bool &brk) {
    cpu.get()->initialize();
    for (;;) {
        if (brk) { break; }
        cpu.get()->next();
    }
}

void MainWindow::open() {
    QString file = QFileDialog::getOpenFileName(this, "Open a ROM", QDir::currentPath(), "*.nes");
    loadCartridge(file.toStdString());
}

void MainWindow::execute() {
    threadCPU(MainWindow::runCPU, cpu, std::reference_wrapper<bool>(breakExecution));
}

void MainWindow::openDebugger() {

}
*/
