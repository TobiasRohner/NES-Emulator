#-------------------------------------------------
#
# Project created by QtCreator 2016-04-14T15:37:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NES
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    Emulator/Memory/memory.cpp \
    Emulator/Memory/Mappers/mapper.cpp \
    Emulator/cpu_6502.cpp \
    Emulator/debugger.cpp \
    Emulator/clock.cpp \
    Emulator/Memory/Mappers/mapper_0.cpp \
    HexEditor/hexedit.cpp

HEADERS  += mainwindow.h \
    Emulator/Memory/memory.h \
    options.h \
    Emulator/Memory/cpumemory.h \
    Emulator/Memory/Mappers/mapper.h \
    Emulator/gamecartridge.h \
    Emulator/Memory/Mappers/mapper_0.h \
    Emulator/Memory/ppumemory.h \
    Emulator/cpu_6502.h \
    Emulator/debugger.h \
    Emulator/clock.h \
    HexEditor/hexedit.h

FORMS    += mainwindow.ui \
    HexEditor/hexedit.ui \
    Emulator/debugger.ui
