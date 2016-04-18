#include "mainwindow.h"
#include <QApplication>
#include <fstream>
#include "../HexEditor/hexedit.h"


std::vector<char> open(const char* path) {
    std::ifstream file(path, std::ios::binary);

    return std::vector<char>((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    std::vector<char> data = open("E:/Programmieren/C++/NES_v2/HexEditor/hexedit.cpp");
    HexEdit hexedit(&data, 0);
    w.setCentralWidget(&hexedit);
    w.show();
    
    return a.exec();
}
