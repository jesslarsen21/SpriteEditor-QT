/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <QObject>
#include <QVector>
#include <QStack>
#include <QSize>
#include <QImage>
#include <QFile>
#include <QColor>
#include <QTextStream>
#include <QMessageBox>
#include <string>
#include <QRect>
#include <cmath>
#include <QDir>
#include <QSysInfo>
#include "pen.h"
#include "animation.h"
#include "dialog.h"
#include <QPainter>

class SpriteSheet : public QObject
{
    Q_OBJECT

public:
    SpriteSheet(QSize size, QSize viewSize);
    ~SpriteSheet();
    Pen pen;

    void addImage(QImage image);
    int getCurrentFrame();
    bool returnModified();
    QImage* getTemp();
    bool isShape();
    void reset();

signals:
    void updateViewRequest(QImage);
    void updateAllViews();
    void stopAnimation();
    void resetAnimation();
    void selectedTool(QString);
    void updateImageSize(QSize);

public slots:
    void save(QString filename);
    void load(QString filename);
    void downloadGIF(QString filename);
    void undo();
    void redo();
    void draw(int x, int y);
    void beginDraw(int x, int y);
    void endDraw(int x, int y);
    void newFrame();
    void deleteFrame();
    void copyFrame();
    void setCurrentFrame(int frame);
    QImage getFrame();
    QImage getPrev();
    QImage getNext();
    QVector<QImage>* getImages();
    void updatePenSize(int);
    void setPen();
    void setEraser();
    void setEyedropper();
    void fill();
    void text();
    void clear();
    void pan();
    void setPenColor(QColor);
    void setMirrorX();
    void setMirrorY();
    void drawRectangle();
    void drawCircle();
    void drawLine();
    void flipX();
    void flipY();
    void rotateRight();
    void rotateLeft();
    void shiftUp();
    void shiftDown();
    void shiftLeft();
    void shiftRight();
    void downloadPNG(QString filename);
    void setDimensions(QSize dim);

private:
    QVector<QImage> mFrames;
    QSize mDimensions;
    QSize mScaledSize;
    bool isModified;
    bool drawingShape;
    QImage temp;
    QStack<QImage> *undoStack;
    QStack<QImage> *redoStack;
    int currentFrame;

    QPoint getPixelCoordinates(int x, int y);
    void setPenMode(PenMode mode);

};

#endif // SPRITESHEET_H
