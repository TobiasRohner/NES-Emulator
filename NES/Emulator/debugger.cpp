#include "debugger.h"
#include "ui_debugger.h"
#include <iostream>
#include "../options.h"


Debugger::Debugger(std::shared_ptr<Mapper> mapper, std::shared_ptr<CPU_6502> cpu, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Debugger)
{
    ui->setupUi(this);
    setupUI();
    connectActions();

    this->mapper = mapper;
    this->cpu = cpu;

    hexeditCPU.setMapper(mapper);
    hexeditPPU.setMapper(mapper);
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

void Debugger::setupUI() {
    ui->displayPC->setPrefix("0x");
    ui->displayPC->setDisplayIntegerBase(16);
    ui->displaySP->setPrefix("0x");
    ui->displaySP->setDisplayIntegerBase(16);
    ui->displayA->setPrefix("0x");
    ui->displayA->setDisplayIntegerBase(16);
    ui->displayX->setPrefix("0x");
    ui->displayX->setDisplayIntegerBase(16);
    ui->displayY->setPrefix("0x");
    ui->displayY->setDisplayIntegerBase(16);
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
    terminateOpcode(cpu->getBridge());
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
