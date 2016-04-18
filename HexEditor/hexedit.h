#ifndef HEXEDIT_H
#define HEXEDIT_H

#include <QAbstractScrollArea>
#include <vector>
#include <stdint.h>
#include <QString>
#include <QFont>
#include <QFontDatabase>
#include <QPainter>

#define BYTES_PER_LINE 16     /*The number of Bytes displayed on a single line*/

namespace AREA {
const int NONE = 0;
const int BYTES = 1;
const int ASCII = 2;
}

namespace Ui {
class HexEdit;
}

struct CursorPosition {
    int byte = 0;
    int nibble = 0;
    int area = AREA::BYTES;
    int pos() const { return 2*byte+nibble; }
    int line() const { return (int)(byte/BYTES_PER_LINE); }
    inline bool operator==(const CursorPosition &cp) { return (byte==cp.byte && nibble==cp.nibble); }
    inline bool operator!=(const CursorPosition &cp) { return !operator==(cp); }
    inline bool operator<(const CursorPosition &cp) { return pos()<cp.pos(); }
    inline bool operator>=(const CursorPosition &cp) { return !operator<(cp); }
    inline bool operator>(const CursorPosition &cp) { return pos()>cp.pos(); }
    inline bool operator<=(const CursorPosition &cp) { return !operator>(cp); }
};

class HexEdit : public QAbstractScrollArea
{
    Q_OBJECT
    
public:
    explicit HexEdit(std::vector<char> *data, QWidget *parent = 0);
    ~HexEdit();
    
private:
    Ui::HexEdit *ui;
    std::vector<char> *data;
    const QFont monospaceFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    const bool ASCII_SHOWN = true;               //If the ASCII version of the File will be shown
    const bool ADDRESS_SHOWN = true;             //If the adresses are shown
    const bool OFFSET_SHOWN = true;              //If the address offsets are shown
    const bool BYTES_SHOWN = true;               //If the bytes are shown
    const unsigned int ADDRESS_DIGITS = 8;       //Number of characters used to represent an address
    const float BYTE_DISTANCE = 1.0;             //The distance in which the single bytes are written apart (measured in char widths)
    const float ADDRESS_BYTE_DISTANCE = 2;       //The distance which the bytes have to the addresses in character widths
    const float BYTES_ASCII_DISTANCE = 2;        //The distance between the bytes and the ascii representation in character widths
    const float OFFSET_DATA_DISTANCE = 0.5;        //The distance between the offset indicators and the data itself
    const int ADDRESS_X_POS = 5;                 //Distance the addresses have from the border in pixels
    const int OFFSET_Y_POS = 5;                  //Distance the offsets have from the top border of the widget in pixels
    unsigned int CHAR_HEIGHT;                       //The height of a single char in pixels
    unsigned int CHAR_WIDTH;                        //The width of a single char in pixels
    unsigned int LINESPACING;

    unsigned int currentLine = 0;                       //The topmost currently visible line
    int xOffset = 0;
    unsigned int linesShown = 10;                       //How many lines are currently visible
    CursorPosition cursorPosition;
    bool cursorActive = false;
    bool leftButtonPressed = false;
    CursorPosition selectionStart;
    CursorPosition selectionEnd;

    CursorPosition indexClicked(int x, int y, bool ignoreBorders=false);
    int cursorXBytes();
    int cursorYBytes();
    int cursorXASCII();
    int cursorYASCII();

    inline QString hex(char value);
    inline QString hexAddress(long address);

    void adjustViewport();
    void adjustScrollBars();

    CursorPosition setCursorPosition(int x, int y, bool ignoreBorders=false);

    void writeNibble(unsigned char value);
    void cursorRight();
    void cursorLeft();
    void cursorUp();
    void cursorDown();

    inline int addressXPos();
    inline int bytesXPos();
    inline int asciiXPos();
    inline int offsetYPos();
    inline int dataYPos();

    void drawAddresses(QPainter *painter);
    void drawAddressOffset(QPainter *painter);
    void drawBytes(QPainter *painter);
    void drawASCII(QPainter *painter);
    void drawSelection(QPainter *painter);
    void drawCursor(QPainter *painter);

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // HEXEDIT_H
