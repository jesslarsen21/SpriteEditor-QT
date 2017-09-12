/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#ifndef QCLICKABLELABEL_H
#define QCLICKABLELABEL_H

#include <QObject>
#include <QLabel>

class QClickableLabel : public QLabel
{
Q_OBJECT
public:
    explicit QClickableLabel(QWidget* parent=0 );
    ~QClickableLabel();
signals:
    void clicked(QString label);

protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // QCLICKABLELABEL_H
