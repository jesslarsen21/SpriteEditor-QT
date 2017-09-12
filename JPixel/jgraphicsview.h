/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#ifndef JGRAPHICSVIEW_H
#define JGRAPHICSVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <cmath>
#include <string>
#include "spritesheet.h"

class JGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    JGraphicsView(QWidget* parent);
    ~JGraphicsView();
    SpriteSheet SS;
    bool onionSkin;

public slots:
    void render();
    void addImage();
    void updateView(QImage x);
    void setOnionSkin(bool);
    void panBy(int x, int y);
    void zoomIn();
    void zoomOut();
    void setImageSize(QSize size);
    
signals:
    void updateSignal();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void panBoundsCheck();
    QPoint screenSpaceToDrawSpace(QPoint loc);
    QImage image;
    bool drawing;
    QGraphicsScene* scene;
    double mZoom;
    double mPanXCenter, mPanYCenter;

private:
    QSize mImgSize;
};

#endif // JGRAPHICSVIEW_H
