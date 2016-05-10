#include "hexedit.h"
#include "ui_hexedit.h"
#include <fstream>
#include <iostream>
#include "qpainter.h"
#include <algorithm>
#include <QScrollBar>
#include <QMouseEvent>
#include <QColor>


HexEdit::HexEdit(std::shared_ptr<Mapper> mapper, QWidget *parent) : QAbstractScrollArea(parent),ui(new Ui::HexEdit)
{
    ui->setupUi(this);

    this->data = mapper;

    QPainter painter(viewport());
    painter.setFont(monospaceFont);
    CHAR_WIDTH = painter.fontMetrics().averageCharWidth()*1.5;
    CHAR_HEIGHT = painter.fontMetrics().height();
    LINESPACING = painter.fontMetrics().lineSpacing()+2;

    adjustViewport();
}

HexEdit::~HexEdit()
{
    delete ui;
}

void HexEdit::setMapper(std::shared_ptr<Mapper> mapper) {
    this->data = mapper;
}

void HexEdit::updateMonitor() {
    viewport()->update();
}

CursorPosition HexEdit::indexClicked(int x, int y, bool ignoreBorders) {
    CursorPosition pos;
    float xBytes = (float)(x-bytesXPos())/(BYTES_PER_LINE*3*CHAR_WIDTH-CHAR_WIDTH);
    float xAscii = (float)(x-asciiXPos())/(BYTES_PER_LINE*CHAR_WIDTH);
    float yData = (float)(y-dataYPos())/(linesShown*LINESPACING);
    int nibble = cursorPosition.nibble;
    int byte = cursorPosition.byte;
    int area = AREA::NONE;

    if (ignoreBorders) {
        area = cursorPosition.area;
    }
    else {
        if (xBytes>=0 && xBytes<=1) { area = AREA::BYTES; }
        if (xAscii>=0 && xAscii<=1) { area = AREA::ASCII; }
        if (area == AREA::NONE) {
            pos.area = area;
            pos.byte = -1;
            pos.nibble = 0;
            return pos;
        }
    }
    int chr = 0;
    int line = 0;
    if (area == AREA::BYTES) {
        chr = std::max(0, std::min((int)(xBytes*3*BYTES_PER_LINE), (int)(3*BYTES_PER_LINE)));
        line = std::max(0, std::min((int)(verticalScrollBar()->value()+(yData*linesShown)), (int)(64*KB/BYTES_PER_LINE)+1))-1;
        byte = line*BYTES_PER_LINE + chr/3;
        nibble = std::min(chr%3, 1);
    }
    if (area == AREA::ASCII) {
        chr = std::max(0, std::min((int)(xAscii*BYTES_PER_LINE), (int)(BYTES_PER_LINE)));
        line = std::max(0, std::min((int)(verticalScrollBar()->value()+(yData*linesShown)), (int)(64*KB/BYTES_PER_LINE)+1))-1;
        byte = line*BYTES_PER_LINE + chr;
        nibble = 0;
    }

    pos.area = area;
    pos.byte = byte;
    pos.nibble = nibble;
    return pos;
}

int HexEdit::cursorXBytes() {
    return (cursorPosition.byte%BYTES_PER_LINE)*3*CHAR_WIDTH + cursorPosition.nibble*CHAR_WIDTH + bytesXPos();
}

int HexEdit::cursorYBytes() {
    return (cursorPosition.byte/BYTES_PER_LINE-verticalScrollBar()->value())*LINESPACING + dataYPos()+CHAR_HEIGHT;
}

int HexEdit::cursorXASCII() {
    return (cursorPosition.byte%BYTES_PER_LINE)*CHAR_WIDTH + asciiXPos();
}

int HexEdit::cursorYASCII() {
    return (cursorPosition.byte/BYTES_PER_LINE-verticalScrollBar()->value())*LINESPACING + dataYPos()+CHAR_HEIGHT;
}

inline QString HexEdit::hex(uint8_t value) {
    return QString("%1").arg(value&0xFF, 2, 16, QChar('0')).toUpper();
}

inline QString HexEdit::hexAddress(long address) {
    return QString("%1").arg(address, ADDRESS_DIGITS, 16, QChar('0')).toUpper();
}

void HexEdit::adjustViewport() {
    linesShown = std::max((int)(height()/LINESPACING)-1, 0);
}

void HexEdit::adjustScrollBars() {
    int lines = 64*KB/BYTES_PER_LINE;
    verticalScrollBar()->setRange(0, lines);
    verticalScrollBar()->setSingleStep(1);
    verticalScrollBar()->setPageStep(linesShown);
    horizontalScrollBar()->setRange(0, asciiXPos() + BYTES_PER_LINE*CHAR_WIDTH - this->width() + 50);
    horizontalScrollBar()->setPageStep(this->width());
}

CursorPosition HexEdit::setCursorPosition(int x, int y, bool ignoreBorders) {
    CursorPosition pos = indexClicked(x, y, ignoreBorders);
    if (pos.byte >= 0) {
        cursorPosition = pos;
        cursorActive = true;
    }
    else {
        cursorActive = false;
    }
    return pos;
}

void HexEdit::writeNibble(uint8_t value) {
    if (cursorPosition.nibble == 0) {
        data.get()->storeCPU(cursorPosition.byte, (data.get()->fetchCPU(cursorPosition.byte)&0x0F) | (value<<4));
    }
    else {
        data.get()->storeCPU(cursorPosition.byte, (data.get()->fetchCPU(cursorPosition.byte)&0xF0) | value);
    }
}

void HexEdit::cursorRight() {
    if (cursorPosition.nibble == 0) {
        cursorPosition.nibble = 1;
    }
    else {
        if (cursorPosition.byte < 64*KB-1) {
            cursorPosition.byte++;
            cursorPosition.nibble = 0;
        }
    }
}

void HexEdit::cursorLeft() {
    if (cursorPosition.nibble == 0) {
        if (cursorPosition.byte > 0) {
            cursorPosition.byte--;
            cursorPosition.nibble = 1;
        }
    }
    else {
        cursorPosition.nibble = 0;
    }
}

void HexEdit::cursorUp() {
    if (cursorPosition.line() > 0) {
        cursorPosition.byte -= BYTES_PER_LINE;
    }
}

void HexEdit::cursorDown() {
    if (cursorPosition.line() < (int)((float)64*KB/BYTES_PER_LINE-0.5)) {
        cursorPosition.byte += BYTES_PER_LINE;
    }
}

inline int HexEdit::addressXPos() {
    return ADDRESS_X_POS;
}

inline int HexEdit::bytesXPos() {
    return addressXPos() + ((float)ADDRESS_DIGITS+ADDRESS_BYTE_DISTANCE)*CHAR_WIDTH;
}

inline int HexEdit::asciiXPos() {
    return bytesXPos() + (2.0+BYTE_DISTANCE)*CHAR_WIDTH*(BYTES_PER_LINE-1)+2*CHAR_WIDTH + BYTES_ASCII_DISTANCE*CHAR_WIDTH;
}

inline int HexEdit::offsetYPos() {
    return OFFSET_Y_POS + CHAR_HEIGHT;
}

inline int HexEdit::dataYPos() {
    return offsetYPos() + OFFSET_DATA_DISTANCE*CHAR_HEIGHT;
}

void HexEdit::drawAddresses(QPainter *painter) {
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setPen(viewport()->palette().color(QPalette::WindowText));
    painter->setFont(monospaceFont);

    QString addresses = QString();
    for (unsigned int addr=verticalScrollBar()->value()*BYTES_PER_LINE ; addr<(verticalScrollBar()->value()+linesShown)*BYTES_PER_LINE ; addr+=BYTES_PER_LINE) {
        if (addr >= 64*KB) { break; }
        addresses.append(hexAddress(addr)+"\n");
    }

    painter->drawText(addressXPos()-xOffset, dataYPos()+CHAR_HEIGHT, ADDRESS_DIGITS*CHAR_WIDTH, linesShown*LINESPACING, 0, addresses);
}

void HexEdit::drawAddressOffset(QPainter *painter) {
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setPen(viewport()->palette().color(QPalette::WindowText));
    painter->setFont(monospaceFont);

    QString offset = QString();
    for (unsigned int o=0 ; o<BYTES_PER_LINE ; o++) {
        offset.append(hex(o)+" ");
    }

    painter->drawText(bytesXPos()-xOffset, offsetYPos(), 3*BYTES_PER_LINE*CHAR_WIDTH, LINESPACING, 0, offset);
}

void HexEdit::drawBytes(QPainter *painter) {
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setPen(viewport()->palette().color(QPalette::WindowText));
    painter->setFont(monospaceFont);

    QString bytes = QString();
    for (unsigned int line=0, addr=BYTES_PER_LINE*verticalScrollBar()->value() ; line<linesShown ; line++) {
        for (unsigned int o=0 ; o<BYTES_PER_LINE ; o++, addr++) {
            if (addr >= 64*KB) { goto endloop; }
            bytes.append(hex(data.get()->fetchCPU(addr))+" ");
        }
        bytes.append("\n");
    }
endloop:

    painter->drawText(bytesXPos()-xOffset, dataYPos()+CHAR_HEIGHT, (BYTES_PER_LINE*3-1)*CHAR_WIDTH, linesShown*LINESPACING, 0, bytes);
}

void HexEdit::drawASCII(QPainter *painter) {
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->setPen(viewport()->palette().color(QPalette::WindowText));
    painter->setFont(monospaceFont);

    QString ascii = QString();
    for (unsigned int line=0, addr=BYTES_PER_LINE*verticalScrollBar()->value() ; line<linesShown ; line++) {
        for (unsigned int o=0 ; o<BYTES_PER_LINE ; o++, addr++) {
            if (addr >= 64*KB) { goto endloop; }
            if (data.get()->fetchCPU(addr)>=32 && data.get()->fetchCPU(addr)<=126) { ascii.append(data.get()->fetchCPU(addr)); }
            else { ascii.append("."); }
        }
        ascii.append("\n");
    }
endloop:

    painter->drawText(asciiXPos()-xOffset, dataYPos()+CHAR_HEIGHT, BYTES_PER_LINE*CHAR_WIDTH, linesShown*LINESPACING, 0, ascii);
}

void HexEdit::drawSelection(QPainter *painter) {
    if (selectionStart != selectionEnd) {
        unsigned int startByte = std::min(selectionStart.byte, selectionEnd.byte);
        unsigned int endByte = std::max(selectionStart.byte, selectionEnd.byte);
        QColor color;

        //on the bytes
        int startXBytes = (startByte%BYTES_PER_LINE)*3*CHAR_WIDTH + bytesXPos();
        int startYBytes = (startByte/BYTES_PER_LINE-verticalScrollBar()->value())*LINESPACING + dataYPos()+CHAR_HEIGHT;
        int endXBytes = (endByte%BYTES_PER_LINE)*3*CHAR_WIDTH + bytesXPos();
        int endYBytes = (endByte/BYTES_PER_LINE-verticalScrollBar()->value())*LINESPACING + dataYPos()+CHAR_HEIGHT;
        if (cursorPosition.area == AREA::BYTES) {
            painter->setCompositionMode(QPainter::RasterOp_SourceOrNotDestination);                                                 //draw over with 0x00 0x00 0xFF
            color = QColor(0x00,0x00,0xFF);
        }
        else {
            painter->setCompositionMode((QPainter::CompositionMode_SourceOver));
            color = QColor(0x80,0x80,0xFF,0x80);
        }
        if (startYBytes == endYBytes) {     //start, end on the same line
            painter->fillRect(startXBytes-xOffset, startYBytes, endXBytes-(startXBytes-xOffset)+3*CHAR_WIDTH, CHAR_HEIGHT, color);
        }
        else {
            painter->fillRect(startXBytes-xOffset, startYBytes, bytesXPos()+3*BYTES_PER_LINE*CHAR_WIDTH-CHAR_WIDTH-startXBytes-xOffset, LINESPACING, color);
            painter->fillRect(bytesXPos()-xOffset, startYBytes+LINESPACING, 3*BYTES_PER_LINE*CHAR_WIDTH-CHAR_WIDTH-xOffset, endYBytes-startYBytes-LINESPACING, color);
            painter->fillRect(bytesXPos()-xOffset, endYBytes, endXBytes-bytesXPos()+3*CHAR_WIDTH-xOffset, CHAR_HEIGHT, color);
        }

        //on the ascii representation
        int startXASCII = (startByte%BYTES_PER_LINE)*CHAR_WIDTH + asciiXPos();
        int startYASCII = (startByte/BYTES_PER_LINE-verticalScrollBar()->value())*LINESPACING + dataYPos()+CHAR_HEIGHT;
        int endXASCII = (endByte%BYTES_PER_LINE)*CHAR_WIDTH + asciiXPos();
        int endYASCII = (endByte/BYTES_PER_LINE-verticalScrollBar()->value())*LINESPACING + dataYPos()+CHAR_HEIGHT;
        if (cursorPosition.area == AREA::ASCII) {
            painter->setCompositionMode(QPainter::RasterOp_SourceOrNotDestination);
            color = QColor(0x00,0x00,0xFF);
        }
        else {
            painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            color = QColor(0x80,0x80,0xFF,0x80);
        }
        if (startYASCII == endYASCII) {     //start, end on the same line
            painter->fillRect(startXASCII-xOffset, startYASCII, endXASCII-(startXASCII-xOffset)+CHAR_WIDTH, CHAR_HEIGHT, color);
        }
        else {
            painter->fillRect(startXASCII-xOffset, startYASCII, asciiXPos()+BYTES_PER_LINE*CHAR_WIDTH-startXASCII-xOffset, LINESPACING, color);
            painter->fillRect(asciiXPos()-xOffset, startYASCII+LINESPACING, BYTES_PER_LINE*CHAR_WIDTH-xOffset, endYASCII-startYASCII-LINESPACING, color);
            painter->fillRect(asciiXPos()-xOffset, endYASCII, endXASCII-asciiXPos()+CHAR_WIDTH-xOffset, CHAR_HEIGHT, color);
        }
    }
}

void HexEdit::drawCursor(QPainter *painter) {
    if (cursorActive) {
        if (cursorYBytes()>=dataYPos()) {
            if (cursorPosition.area == AREA::BYTES) {
                painter->setCompositionMode(QPainter::CompositionMode_Source);
                painter->setPen(Qt::DotLine);
                painter->drawRect(cursorXASCII()-xOffset, cursorYASCII(), CHAR_WIDTH, CHAR_HEIGHT);
                painter->setCompositionMode(QPainter::RasterOp_SourceXorDestination);
                painter->fillRect(cursorXBytes()-xOffset, cursorYBytes(), CHAR_WIDTH, CHAR_HEIGHT, QColor(0xFF,0xFF,0xFF));
            }
            else {
                painter->setCompositionMode(QPainter::RasterOp_SourceXorDestination);
                painter->fillRect(cursorXASCII()-xOffset, cursorYASCII(), CHAR_WIDTH, CHAR_HEIGHT, QColor(0xFF,0xFF,0xFF));
                painter->setCompositionMode(QPainter::CompositionMode_Source);
                painter->setPen(Qt::DotLine);
                painter->drawRect(cursorXBytes()-xOffset, cursorYBytes(), CHAR_WIDTH, CHAR_HEIGHT);
            }
        }
    }
}

void HexEdit::paintEvent(QPaintEvent *event) {
    QPainter painter(viewport());
    xOffset = horizontalScrollBar()->value();   //offset in the X-Axis in pixels

    painter.setPen(viewport()->palette().color(QPalette::WindowText));
    painter.setFont(monospaceFont);

    drawAddresses(&painter);
    drawAddressOffset(&painter);
    drawBytes(&painter);
    drawASCII(&painter);
    drawSelection(&painter);
    drawCursor(&painter);
}

void HexEdit::resizeEvent(QResizeEvent *event) {
    adjustViewport();
    adjustScrollBars();
}

void HexEdit::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        leftButtonPressed = true;
        selectionStart = selectionEnd = setCursorPosition(event->x(), event->y());
        updateMonitor();
    }
}

void HexEdit::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        leftButtonPressed = false;
    }
}

void HexEdit::mouseMoveEvent(QMouseEvent *event) {
    if (leftButtonPressed) {
        selectionEnd = setCursorPosition(event->x(), event->y(), true);
        updateMonitor();
    }
}

void HexEdit::keyPressEvent(QKeyEvent *event) {
    if ((event->key() != Qt::Key_Left) &&
        (event->key() != Qt::Key_Right) &&
        (event->key() != Qt::Key_Up) &&
        (event->key() != Qt::Key_Down)) {
        if (cursorPosition.area == AREA::BYTES) {
            switch (event->key()) {
                case Qt::Key_0:
                    writeNibble(0x0);
                    cursorRight();
                    break;
                case Qt::Key_1:
                    writeNibble(0x1);
                    cursorRight();
                    break;
                case Qt::Key_2:
                    writeNibble(0x2);
                    cursorRight();
                    break;
                case Qt::Key_3:
                    writeNibble(0x3);
                    cursorRight();
                    break;
                case Qt::Key_4:
                    writeNibble(0x4);
                    cursorRight();
                    break;
                case Qt::Key_5:
                    writeNibble(0x5);
                    cursorRight();
                    break;
                case Qt::Key_6:
                    writeNibble(0x6);
                    cursorRight();
                    break;
                case Qt::Key_7:
                    writeNibble(0x7);
                    cursorRight();
                    break;
                case Qt::Key_8:
                    writeNibble(0x8);
                    cursorRight();
                    break;
                case Qt::Key_9:
                    writeNibble(0x9);
                    cursorRight();
                    break;
                case Qt::Key_A:
                    writeNibble(0xA);
                    cursorRight();
                    break;
                case Qt::Key_B:
                    writeNibble(0xB);
                    cursorRight();
                    break;
                case Qt::Key_C:
                    writeNibble(0xC);
                    cursorRight();
                    break;
                case Qt::Key_D:
                    writeNibble(0xD);
                    cursorRight();
                    break;
                case Qt::Key_E:
                    writeNibble(0xE);
                    cursorRight();
                    break;
                case Qt::Key_F:
                    writeNibble(0xF);
                    cursorRight();
                    break;
                default:
                    break;
            }
        }
        if (cursorPosition.area == AREA::ASCII) {
            if (!(event->text().isEmpty())) {
                char d = event->text().toStdString().c_str()[0];
                data.get()->storeCPU(cursorPosition.byte, d);
                cursorRight();
                cursorRight();
            }
        }
    }
    switch (event->key()) {
        case Qt::Key_Right:
            if (cursorPosition.area == AREA::BYTES) { cursorRight(); }
            if (cursorPosition.area == AREA::ASCII) { cursorRight(); cursorRight(); }
            break;
        case Qt::Key_Left:
            if (cursorPosition.area == AREA::BYTES) { cursorLeft(); }
            if (cursorPosition.area == AREA::ASCII) { cursorLeft(); cursorLeft(); }
            break;
        case Qt::Key_Up:
            cursorUp();
            break;
        case Qt::Key_Down:
            cursorDown();
            break;
        default:
            break;
    }
    updateMonitor();
}
