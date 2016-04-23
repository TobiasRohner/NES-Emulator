#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QMainWindow>
#include <memory>
#include <condition_variable>
#include "Memory/Mappers/mapper.h"
#include "../HexEditor/hexedit.h"
#include "clock.h"
#include "cpu_6502.h"

namespace Ui {
class Debugger;
}

class Debugger : public QMainWindow
{
    Q_OBJECT

public:
    explicit Debugger(std::shared_ptr<Mapper> mapper, std::shared_ptr<COMMUNCATION_BRIDGE> bridge, CPU_6502 *cpu, QWidget *parent = 0);
    ~Debugger();

private:
    Ui::Debugger *ui;

    std::shared_ptr<COMMUNCATION_BRIDGE> bridge;
    std::shared_ptr<Mapper> mapper;
    CPU_6502 *cpu;

    HexEdit hexeditCPU;
    HexEdit hexeditPPU;

    void connectActions();

    void updateDisplayPC();
    void updateDisplaySP();
    void updateDisplayA();
    void updateDisplayX();
    void updateDisplayY();
    void updateAllRegisterDisplays();

private slots:
    void pause();
    void next();
    void cont();    //continue
};

#endif // DEBUGGER_H
