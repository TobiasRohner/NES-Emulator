#include "debugger.h"
#include "ui_debugger.h"
#include <iostream>
#include "../options.h"


Debugger::Debugger(std::shared_ptr<Mapper> mapper, std::shared_ptr<COMMUNCATION_BRIDGE> bridge, CPU_6502 *cpu, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Debugger)
{
    ui->setupUi(this);
    connectActions();

    this->mapper = mapper;
    this->bridge = bridge;
    this->cpu = cpu;

    hexeditCPU.setData(mapper->memoryCPU.getData());
    hexeditPPU.setData(mapper->memoryPPU.getData());
    ui->layoutCPU->addWidget(&hexeditCPU);
    ui->layoutPPU->addWidget(&hexeditPPU);

    updateAllRegisterDisplays();
    hexeditCPU.updateMonitor();
    hexeditPPU.updateMonitor();
}

Debugger::~Debugger()
{
    delete ui;
}

void Debugger::connectActions() {
    connect(ui->actionPause, &QAction::triggered, this, &Debugger::pause);
    connect(ui->actionNext, &QAction::triggered, this, &Debugger::next);
    connect(ui->actionContinue, &QAction::triggered, this, &Debugger::cont);
}

void Debugger::pause() {
#ifdef DEBUG
    std::cout << "pause" << std::endl;
#endif
}

void Debugger::next() {
#ifdef DEBUG
    std::cout << "next" << std::endl;
#endif
    terminateOpcode(bridge);
    hexeditCPU.updateMonitor();
    updateAllRegisterDisplays();
}

void Debugger::cont() {
#ifdef DEBUG
    std::cout << "continue" << std::endl;
#endif
}

void Debugger::updateDisplayPC() {
    ui->displayPC->setValue(cpu->getPC());
}

void Debugger::updateDisplaySP() {
    ui->displaySP->setValue(cpu->getSP());
}

void Debugger::updateDisplayA() {
    ui->displayA->setValue(cpu->getA());
}

void Debugger::updateDisplayX() {
    ui->displayX->setValue(cpu->getX());
}

void Debugger::updateDisplayY() {
    ui->displayY->setValue(cpu->getY());
}

void Debugger::updateAllRegisterDisplays() {
    updateDisplayPC();
    updateDisplaySP();
    updateDisplayA();
    updateDisplayX();
    updateDisplayY();
}
