/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QImage>
#include <QString>

class Animation : public QObject
{
    Q_OBJECT

public:
    Animation();
    ~Animation();
    void setup(QVector<QImage>* framePtr);
    bool isPlaying();

signals:
    void displayImage(QImage image);
    void animationStatus(QString);

private slots:
    void play(int);
    void pause();
    void restart();
    void updateImage();
    void updateTimer(int);
    void reset();

private:
    QVector<QImage>* framePtr;
    int interval;
    QImage currentImage;
    int currentImageIndex;
    QTimer* timerPtr;
    bool playing;
};

#endif // ANIMATION_H
