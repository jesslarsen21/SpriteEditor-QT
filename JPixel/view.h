/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#ifndef VIEW_H
#define VIEW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QColorDialog>
#include <QColor>
#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <cmath>
#include <QFileDialog>
#include <QInputDialog>
#include <QComboBox>
#include "pen.h"
#include "jgraphicsview.h"
#include "spritesheet.h"
#include "animation.h"

namespace Ui {
class View;
}

class View : public QMainWindow
{
    Q_OBJECT

public:
    explicit View(QWidget *parent = 0);
    ~View();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::View *ui;
    Dialog popup;
    Animation mAnimation;
    SpriteSheet* SSPtr;
    QString filename;
    QImage colorPrev;
    bool fileCancelled;

private slots:
    void fileOpen();
    void fileSave();
    void fileClose();
    void fileExport();
    void updateCurrentTool(QString);
    void updateFromEyedrop(QColor color);
    void on_pickColorPushButton_clicked();
    void showDialog();
    void respondToClose(std::string input);
    void updatePreviewLabel(QImage);
    void updatePreviewButton(QString);
    void getFPS();
    void newSprite();
    void frameClick(QString frame);
    void deleteFrame();
    void updateFramesPreviewBox();
    void clearPreviewFrames();
    void PngExport();

signals:
    void colorChosen(QColor);
    void startAnimation(int);
    void fileLoad(QString);
    void fileSave(QString);
    void exportGIF(QString);
    void updateViews();
    void resetAll();
    void pauseAnimation();
    void resetAnimation();
    void deleteFrameSS();
    void restartAnimation();
    void changeImageSize(QSize);
    void exportPNG(QString);

private:
    void reset();
    void setTitle();
    int getImageSize();


};

#endif // VIEW_H
