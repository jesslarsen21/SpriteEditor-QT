/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#include "qclickablelabel.h"

QClickableLabel::QClickableLabel( QWidget* parent)
    : QLabel(parent)
{
}

QClickableLabel::~QClickableLabel()
{
}

void QClickableLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked(this->objectName());
}
