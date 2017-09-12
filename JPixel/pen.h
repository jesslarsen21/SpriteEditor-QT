/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#ifndef PEN_H
#define PEN_H

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QImage>
#include <QList>
#include <QPainter>
#include <cmath>

enum PenMode
{
    PenMode_DotsOnly,
    PenMode_Pencil,
    PenMode_Eraser,
    PenMode_Eyedropper,
    PenMode_Fill,
    PenMode_Text,
    PenMode_Line,
    PenMode_Rectangle,
    PenMode_Circle,
    PenMode_MirrorXAxis,
    PenMode_MirrorYAxis,
    PenMode_Pan
};

class Pen : public QObject
{
    Q_OBJECT

private:
    int mSize;
    QColor mColor;
    PenMode mMode;
    QPoint mFirstPoint;
    QPoint mPreviousPoint;

public:
    Pen();
    void beginDraw(QImage* img, int x, int y);
    void draw(QImage* img, int x, int y);
    void endDraw(QImage* img, int x, int y);
    void fill(QImage* img, int x, int y);
    void drawShape(QImage* img, std::string str, int x0, int y0, int x1, int y1);
    void redrawShape(QImage* img, std::string str, int x, int y);
    void setColor(QColor color);
    QColor getColor();
    void setMode(PenMode mode);
    PenMode getMode();
    void addText(QImage* image, std::string input);
    void penSize(int);
    void drawA(QImage* image, int x, int y);
    void drawB(QImage* image, int x, int y);
    void drawC(QImage* image, int x, int y);
    void drawD(QImage* image, int x, int y);
    void drawE(QImage* image, int x, int y);
    void drawF(QImage* image, int x, int y);
    void drawG(QImage* image, int x, int y);
    void drawH(QImage* image, int x, int y);
    void drawI(QImage* image, int x, int y);
    void drawJ(QImage* image, int x, int y);
    void drawK(QImage* image, int x, int y);
    void drawL(QImage* image, int x, int y);
    void drawM(QImage* image, int x, int y);
    void drawN(QImage* image, int x, int y);
    void drawO(QImage* image, int x, int y);
    void drawP(QImage* image, int x, int y);
    void drawQ(QImage* image, int x, int y);
    void drawR(QImage* image, int x, int y);
    void drawS(QImage* image, int x, int y);
    void drawT(QImage* image, int x, int y);
    void drawU(QImage* image, int x, int y);
    void drawV(QImage* image, int x, int y);
    void drawW(QImage* image, int x, int y);
    void drawX(QImage* image, int x, int y);
    void drawY(QImage* image, int x, int y);
    void drawZ(QImage* image, int x, int y);
    void draw0(QImage* image, int x, int y);
    void draw1(QImage* image, int x, int y);
    void draw2(QImage* image, int x, int y);
    void draw3(QImage* image, int x, int y);
    void draw4(QImage* image, int x, int y);
    void draw5(QImage* image, int x, int y);
    void draw6(QImage* image, int x, int y);
    void draw7(QImage* image, int x, int y);
    void draw8(QImage* image, int x, int y);
    void draw9(QImage* image, int x, int y);
    void drawPeriod(QImage* image, int x, int y);
    void drawExclamation(QImage* image, int x, int y);
    void drawQuestionMark(QImage* image, int x, int y);
    void drawSymbol(char c, QImage* image, int x, int y);
    void drawUnsupportedSymbol(QImage* image, int x, int y);
    void fillPixel(QImage* image, int currX, int currY);

signals:
    void showPopup();
    void panBy(int x, int y);
    void updateColorDisplay(QColor);
};

#endif // PEN_H
