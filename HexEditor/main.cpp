#include "hexedit.h"
#include <QApplication>
#include <fstream>
#include <iterator>
#include <vector>



std::vector<char> open(const char* path) {
    std::ifstream file(path, std::ios::binary);

    return std::vector<char>((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    std::vector<char> data = open("E:/Programmieren/C++/NES/nestest.nes");
    std::vector<char> data = open("E:/Programmieren/C++/NES_v2/HexEditor/hexedit.cpp");
    HexEdit w(&data, 0);

    w.show();
    
    return a.exec();
}
