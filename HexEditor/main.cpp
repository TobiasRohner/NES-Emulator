#include "hexedit.h"
#include <QApplication>
#include <fstream>
#include <iterator>
#include <vector>



std::vector<uint8_t> openBytes(const char* path) {
    std::ifstream file(path, std::ios::binary);

    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    std::vector<char> data = open("E:/Programmieren/C++/NES/nestest.nes");
    std::vector<uint8_t> data = openBytes("E:/Programmieren/C++/NES_v2/HexEditor/hexedit.cpp");
    HexEdit w(&data, 0);

    w.show();
    
    return a.exec();
}
