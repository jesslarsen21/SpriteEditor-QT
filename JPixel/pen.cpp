/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#include "pen.h"


/*Constructor */
Pen::Pen():
    mSize(1),
    mColor(Qt::black),
    mMode(PenMode_Pencil)
{
}


/* Fired when a drawing event begins (e.g. when the mouse is first clicked) */
void Pen::beginDraw(QImage* imgPtr, int x, int y)
{
    // Store the first point
    mPreviousPoint = QPoint(x, y);
    mFirstPoint = QPoint(x, y);

    // some drawing modes require info on startup...
    switch(mMode)
    {
    case PenMode_DotsOnly:
        imgPtr->setPixelColor(x, y, mColor);
        break;

    case PenMode_Line:
        drawShape(imgPtr, "Line", x, y, x, y);
        break;

    case PenMode_Rectangle:
        drawShape(imgPtr, "Rect", x, y, x, y);
        break;

    case PenMode_Circle:
        drawShape(imgPtr, "Circ", x, y, x, y);
        break;

    default:
        break;
    }
}


/* Fired when a drawing event changes (e.g. when the mouse moves while the button is being held down) */
void Pen::draw(QImage* imgPtr, int x, int y)
{
    // paint or draw since the mouse has moved
    QPainter painter(imgPtr);
    QPoint currentPoint(x, y);
    QPen qp = painter.pen();
    qp.setColor(mColor);
    qp.setWidth(mSize);
    painter.setPen(qp);


    // each mode has a different method of drawing. Apply it.
    switch(mMode)
    {
    case PenMode_DotsOnly:
        imgPtr->setPixelColor(x, y, mColor);
        break;

    case PenMode_Pencil:
        painter.setBrush(mColor);

        painter.drawLine(mPreviousPoint, currentPoint);
        break;

    case PenMode_Eraser:
        painter.setBrush(QColor(255, 255, 255, 0));

        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.drawLine(mPreviousPoint, currentPoint);
        break;

    case PenMode_Line:
        painter.end();
        redrawShape(imgPtr, "Line", x, y);
        break;

    case PenMode_Rectangle:
        painter.end();
        redrawShape(imgPtr, "Rect", x, y);
        break;

    case PenMode_Circle:
        painter.end();
        redrawShape(imgPtr, "Circ", x, y);
        break;

    case PenMode_MirrorXAxis:
    {
        QPoint mirror0 = QPoint(mPreviousPoint.x(), imgPtr->height() - mPreviousPoint.y() - 1);
        QPoint mirror1 = QPoint(currentPoint.x(), imgPtr->height() - currentPoint.y() - 1);
        painter.drawLine(mPreviousPoint, currentPoint);
        painter.drawLine(mirror0, mirror1);
    }
        break;

    case PenMode_MirrorYAxis:
    {
        QPoint mirror0 = QPoint(imgPtr->width() - mPreviousPoint.x() - 1, mPreviousPoint.y());
        QPoint mirror1 = QPoint(imgPtr->width() - currentPoint.x() - 1, currentPoint.y());
        painter.drawLine(mPreviousPoint, currentPoint);
        painter.drawLine(mirror0, mirror1);
    }
        break;

    case PenMode_Pan:
        panBy(mFirstPoint.x()-x, mFirstPoint.y()-y);
        break;

    default:
        break;
    }

    mPreviousPoint = QPoint(x, y);
}


/* Fired when a drawing event ends (e.g. when the mouse click is released) */
void Pen::endDraw(QImage* imgPtr, int x, int y)
{
    mPreviousPoint = QPoint(x, y);

    switch(mMode)
    {
    case PenMode_DotsOnly:
    case PenMode_Pencil:
    case PenMode_Eraser:
        draw(imgPtr, x, y);
        break;

    case PenMode_Eyedropper:
    {
        QColor pxColor = imgPtr->pixelColor(QPoint(x, y));
        if (pxColor.alpha() != 0)
        {
            mColor = pxColor;
            emit updateColorDisplay(mColor);
        }
    }
        break;

    case PenMode_Fill:
        fill(imgPtr, x, y);
        break;

    case PenMode_Text:
        emit showPopup();
        break;

    case PenMode_Line:
        redrawShape(imgPtr, "Line", x, y);
        break;

    case PenMode_Rectangle:
        redrawShape(imgPtr, "Rect", x, y);
        break;

    case PenMode_Circle:
        redrawShape(imgPtr, "Circ", x, y);
        break;

    case PenMode_MirrorXAxis:
    case PenMode_MirrorYAxis:
        draw(imgPtr, x, y);
        break;

    default:
        break;
    }
}

/* Fills a single-colored block of an image with the current color, beginning at (x, y)*/
void Pen::fill(QImage* imgPtr, int x, int y)
{
    // Do a DFS on everything of the selected color to fill
    QList<QPoint> dfsStack; // Fun fact, QQueue is just a wrapper of QList and has terrible performance
    dfsStack.append(QPoint(x, y));
    QColor oldColor = imgPtr->pixelColor(x, y);

    bool ignoreAlpha = oldColor.alpha() == 0;

    QPainter painter(imgPtr);

    if (oldColor == mColor) {
        // already filled with that color...
        return;
    }

    while (! dfsStack.empty())
    { // Do a DFS

        QPoint pt = dfsStack.takeLast();

        // Mark as visited (and perform the useful work)
        imgPtr->setPixelColor(pt.x(), pt.y(), mColor);


        // check each neighbor, and ignore exact color if both are completely transparent
        if (pt.x() < imgPtr->width()-1 && ((imgPtr->pixelColor(pt.x()+1, pt.y()) == oldColor) || (ignoreAlpha&&(imgPtr->pixelColor(pt.x()+1, pt.y()).alpha() == 0))))
        {
            dfsStack.append(QPoint(pt.x()+1, pt.y()));
        }

        if (pt.x() > 0 && ((imgPtr->pixelColor(pt.x()-1, pt.y()) == oldColor) || (ignoreAlpha&&(imgPtr->pixelColor(pt.x()-1, pt.y()).alpha() == 0))))
        {
            dfsStack.append(QPoint(pt.x()-1, pt.y()));
        }

        if (pt.y() < imgPtr->height()-1 && ((imgPtr->pixelColor(pt.x(), pt.y()+1) == oldColor) || (ignoreAlpha&&(imgPtr->pixelColor(pt.x(), pt.y()+1).alpha() == 0))))
        {
            dfsStack.append(QPoint(pt.x(), pt.y()+1));
        }

        if (pt.y() > 0 && ((imgPtr->pixelColor(pt.x(), pt.y()-1) == oldColor) || (ignoreAlpha&&(imgPtr->pixelColor(pt.x(), pt.y()-1).alpha() == 0))))
        {
            dfsStack.append(QPoint(pt.x(), pt.y()-1));
        }
    }
}

/* Draws a shape; either a line, circle, or rectangle */
void Pen::drawShape(QImage* imgPtr, std::string str, int x0, int y0, int x1, int y1)
{
    mFirstPoint = QPoint(x0, y0);
    //set up
    QPainter painter(imgPtr);
    QPen p(mColor);
    p.setWidth(mSize);
    painter.setPen(p);

    mPreviousPoint = QPoint(x1, y1);

    //determine shape to draw
    if (str == "Line")
    {
        //draw line
        painter.drawLine(mFirstPoint, mPreviousPoint);
    }
    else if (str == "Rect")
    {
        //draw rectangle
        painter.drawLine(mFirstPoint.x(), mFirstPoint.y(), mPreviousPoint.x(), mFirstPoint.y());
        painter.drawLine(mPreviousPoint.x(), mFirstPoint.y(), mPreviousPoint.x(), mPreviousPoint.y());
        painter.drawLine(mFirstPoint.x(), mFirstPoint.y(), mFirstPoint.x(), mPreviousPoint.y());
        painter.drawLine(mFirstPoint.x(), mPreviousPoint.y(), mPreviousPoint.x(), mPreviousPoint.y());
    }
    else if (str == "Circ")
    {
        //draw circle
        int radius = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
        painter.drawEllipse(mFirstPoint, radius, radius);
    }
}

/* Redraws a given shape */
void Pen::redrawShape(QImage* imgPtr, std::string str, int x, int y)
{
    QPainter painter(imgPtr);
    QPen p;
    painter.setBrush(QColor(255, 255, 255, 0));
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setWidth(mSize);
    painter.setPen(p);

    //erase previous shape
    if (str == "Line")
    {
        painter.drawLine(mFirstPoint, mPreviousPoint);
    }
    else if (str == "Rect")
    {
        painter.drawLine(mFirstPoint.x(), mFirstPoint.y(), mPreviousPoint.x(), mFirstPoint.y());
        painter.drawLine(mPreviousPoint.x(), mFirstPoint.y(), mPreviousPoint.x(), mPreviousPoint.y());
        painter.drawLine(mFirstPoint.x(), mFirstPoint.y(), mFirstPoint.x(), mPreviousPoint.y());
        painter.drawLine(mFirstPoint.x(), mPreviousPoint.y(), mPreviousPoint.x(), mPreviousPoint.y());
    }
    else if (str == "Circ")
    {
        int radius = sqrt(pow(mPreviousPoint.x() - mFirstPoint.x(), 2) + pow(mPreviousPoint.y() - mFirstPoint.y(), 2));
        painter.drawEllipse(mFirstPoint, radius, radius);
    }
    painter.end();
    //draw new shape
    drawShape(imgPtr, str, mFirstPoint.x(), mFirstPoint.y(), x, y);
}

/* Sets the current pen color */
void Pen::setColor(QColor color)
{
    mColor = color;
}

/* Gets the current pen color */
QColor Pen::getColor()
{
    return mColor;
}

/* Sets the current pen mode */
void Pen::setMode(PenMode mode)
{
    mMode = mode;
}

/* Gets the current pen mode */
PenMode Pen::getMode()
{
    return mMode;
}

/* Sets the current pen size */
void Pen::penSize(int i)
{
    this->mSize = i;
}

/*  Begins drawing text at the selected pixel. Draws as many symbols
    as possible from the given input. If a full symbol can not fit with regards to space, will draw as
    much of the partial symbol as possible. Supports A-Z, 0-9, .!? - all other symbols will be displayed
    as a filled black square. */
void Pen::addText(QImage* imagePtr, std::string input)
{
    int topLeftX = mPreviousPoint.x();
    int topLeftY = mPreviousPoint.y();

    // Transfer all letters to uppercase + call matching drawing function
    for (int i = 0; i < input.length(); i++)
    {
        char c = toupper(input[i]);
        if (!isspace(c))
        {
            drawSymbol(c, imagePtr, topLeftX, topLeftY);
        }
        // Update topLeft x
        topLeftX += 6;
    }
}

/* Draws a given symbol */
void Pen::drawSymbol(char c, QImage* imagePtr, int x, int y)
{
    if (c == 'A')
	{
        drawA(imagePtr, x, y);
	}
    else if (c == 'B')
	{
        drawB(imagePtr, x, y);
	}
    else if (c == 'C')
	{
        drawC(imagePtr, x, y);
	}
    else if (c == 'D')
	{
        drawD(imagePtr, x, y);
	}
    else if (c == 'E')
	{
        drawE(imagePtr, x, y);
	}
    else if (c == 'F')
	{
        drawF(imagePtr, x, y);
	}
    else if (c == 'G')
	{
        drawG(imagePtr, x, y);
	}
    else if (c == 'H')
	{
        drawH(imagePtr, x, y);
	}
    else if (c == 'I')
	{
        drawI(imagePtr, x, y);
	}
    else if (c == 'J')
	{
        drawJ(imagePtr, x, y);
	}
    else if (c == 'K')
	{
        drawK(imagePtr, x, y);
	}
    else if (c == 'L')
	{
        drawL(imagePtr, x, y);
	}
    else if (c == 'M')
	{
        drawM(imagePtr, x, y);
	}
    else if (c == 'N')
	{
        drawN(imagePtr, x, y);
	}
    else if (c == 'O')
	{
        drawO(imagePtr, x, y);
	}
    else if (c == 'P')
	{
        drawP(imagePtr, x, y);
	}
    else if (c == 'Q')
	{
        drawQ(imagePtr, x, y);
	}
    else if (c == 'R')
	{
        drawR(imagePtr, x, y);
	}
    else if (c == 'S')
	{
        drawS(imagePtr, x, y);
	}
    else if (c == 'T')
	{
        drawT(imagePtr, x, y);
	}
    else if (c == 'U')
	{
        drawU(imagePtr, x, y);
	}
    else if (c == 'V')
	{
        drawV(imagePtr, x, y);
	}
    else if (c == 'W')
	{
        drawW(imagePtr, x, y);
	}
    else if (c == 'X')
	{
        drawX(imagePtr, x, y);
	}
    else if (c == 'Y')
	{
        drawY(imagePtr, x, y);
	}
    else if (c == 'Z')
	{
        drawZ(imagePtr, x, y);
	}
    else if (c == '0')
	{
        draw0(imagePtr, x, y);
	}
    else if (c == '1')
	{
        draw1(imagePtr, x, y);
	}
    else if (c == '2')
	{
        draw2(imagePtr, x, y);
	}
    else if (c == '3')
	{
        draw3(imagePtr, x, y);
	}
    else if (c == '4')
	{
        draw4(imagePtr, x, y);
	}
    else if (c == '5')
	{
        draw5(imagePtr, x, y);
	}
    else if (c == '6')
	{
        draw6(imagePtr, x, y);
	}
    else if (c == '7')
	{
        draw7(imagePtr, x, y);
	}
    else if (c == '8')
	{
        draw8(imagePtr, x, y);
	}
    else if (c == '9')
	{
        draw9(imagePtr, x, y);
	}
    else if (c == '.')
	{
        drawPeriod(imagePtr, x, y);
	}
    else if (c == '!')
	{
        drawExclamation(imagePtr, x, y);
	}
    else if (c == '?')
	{
        drawQuestionMark(imagePtr, x, y);
	}
    else
	{
        drawUnsupportedSymbol(imagePtr, x, y);
	}
}

/* Draws an "unsupported" symbol, a black square */
void Pen::drawUnsupportedSymbol(QImage* imagePtr, int x, int y)
{
    // Iterate through y-axis
    for (int currY = y; currY < y + 5; currY++)
    {
        // Iterate through x-axis
        for (int currX = x; currX < x + 5; currX++)
        {
            fillPixel(imagePtr, currX, currY);
        }
    }
}

/* The following draw the symbol listed in the method name: */
void Pen::drawA(QImage* imagePtr, int x, int y)
{
    // Iterate through 5x5 and fill in A pattern
    int currX = x;
    int currY = y;

    // Fill in top line XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXXX
    currY += 2;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY +=3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X...X
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawB(QImage* imagePtr, int x, int y)
{
    // Iterate through 5x5 and fill in B pattern
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXX.
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
       currX++;
    fillPixel(imagePtr, currX, currY);
        currX++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXX.
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
       currX++;
    fillPixel(imagePtr, currX, currY);
        currX++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXX.
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
       currX++;
    fillPixel(imagePtr, currX, currY);
        currX++;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawC(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X....
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X....
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X....
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXXX
    currY += 4;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawD(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXX.
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X...X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXX.
    currY += 4;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawE(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X....
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXXX
    currY += 2;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X....
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXXX
    currY += 4;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawF(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X....
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXXX
    currY += 2;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X....
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X....
    currY += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawG(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXXX
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X....
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X.XXX
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX +=4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .XXXX
    currY += 4;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawH(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXXX
    currY += 2;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X...X
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawI(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXX.
    currX++;;
    fillPixel(imagePtr, currX, currY);
    currX++;;
    fillPixel(imagePtr, currX, currY);
    currX++;;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 ..X..
    currY++;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..X..
    currY += 2;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 ..X..
    currY += 3;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .XXX.
    currY += 4;
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX++;;
    fillPixel(imagePtr, currX, currY);
    currX++;;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawJ(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 ....X
    currY++;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ....X
    currY += 2;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .XXX.
    currY += 4;
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawK(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X..X.
    fillPixel(imagePtr, currX, currY);
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 X.X..
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXX..
    currY += 2;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X..X.
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X...X
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawL(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X....
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 X....
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X....
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X....
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXXX
    currY += 4;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawM(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 XX.XX
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X.X.X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X...X
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawN(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 XX..X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X.X.X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X..XX
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X...X
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawO(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXX.
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X...X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .XXX.
    currY += 4;
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawP(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXX.
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXX.
    currY += 2;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X....
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X....
    currY += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawQ(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXX.
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X...X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X..XX
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .XXXX
    currY += 4;
    currX++;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawR(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXX.
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X...X
    currY += 2;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X.X..
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X..X.
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX += 3;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawS(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXXX
    currX++;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X....
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXXX
    currY += 2;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 ....X
    currY += 3;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXX.
    currY += 4;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawT(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 ..X..
    currY++;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..X..
    currY += 2;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 ..X..
    currY += 3;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 ..X..
    currY += 4;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawU(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X...X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .XXX.
    currY += 4;
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawV(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X...X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 .X.X.
    currY += 3;
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 ..X..
    currY += 4;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawW(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X.X.X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 XX.XX
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X...X
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawX(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in top line X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 .X.X.
    currX++;
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..X..
    currX += 2;
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 .X.X.
    currY +=3;
    currX += 1;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X...X
    currY += 4;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawY(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in top line X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 .X.X.
    currX++;
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..X..
    currX += 2;
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 .X...
    currY +=3;
    currX += 1;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X....
    currY += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawZ(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 ...X.
    currY++;
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..X..
    currX += 2;
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 .X...
    currY +=3;
    currX += 1;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X....
    currY += 4;
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::draw0(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXX.
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X..XX
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 X.X.X
    currY += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 XX..X
    currY +=3;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 X....
    currY += 4;
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::draw1(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 ..X..
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 .XX..
    currY++;
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..X..
    currY += 2;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 ..X..
    currY +=3;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 ..X..
    currY += 4;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
}

void Pen::draw2(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXX.
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 ....X
    currY++;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 .XXX.
    currY += 2;
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X....
    currY +=3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 ..X..
    currY += 4;
    currX++;;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::draw3(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXX.
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 ....X
    currY++;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..XXX
    currY += 2;
    currX += 2;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 ....X
    currY += 3;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXX.
    currY += 4;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::draw4(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 X...X
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 ....X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 .XXXX
    currY += 2;
    currX++;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 ....X
    currY += 3;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 ....X
    currY += 4;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::draw5(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X....
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXX.
    currY += 2;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 ....X
    currY += 3;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXX.
    currY += 4;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::draw6(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXXX
    currX++;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X....
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 XXXX.
    currY += 2;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .XXX.
    currY += 4;
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::draw7(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 ....X
    currY++;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ...X.
    currY += 2;
    currX += 3;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 4 ..X..
    currY += 3;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .X...
    currY += 4;
    currX++;
    fillPixel(imagePtr, currX, currY);
}

void Pen::draw8(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXX.
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 .XXX.
    currY += 2;
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 X...X
    currY += 3;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 .XXX.
    currY += 4;
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::draw9(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 .XXX.
    currX++;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 X...X
    currY++;
    fillPixel(imagePtr, currX, currY);
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 .XXXX
    currY += 2;
    currX++;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 ....X
    currY += 3;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 XXXX.
    currY += 4;
    for (int i = 0; i < 4; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
}

void Pen::drawPeriod(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 5 ...X.
    currY += 4;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawExclamation(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 ..X..
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 2 ..X..
    currY++;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..X..
    currY += 2;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 ..X..
    currY += 4;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
}

void Pen::drawQuestionMark(QImage* imagePtr, int x, int y)
{
    int currX = x;
    int currY = y;

    // Fill in line 1 XXXXX
    for (int i = 0; i < 5; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 2 ....X
    currY++;
    currX += 4;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 3 ..XXX
    currY += 2;
    currX += 2;
    for (int i = 0; i < 3; i++)
    {
        fillPixel(imagePtr, currX, currY);
        currX++;
    }
    currX = x;
    currY = y;

    // Fill in line 4 ..X..
    currY += 3;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
    currX = x;
    currY = y;

    // Fill in line 5 ..X..
    currY += 4;
    currX += 2;
    fillPixel(imagePtr, currX, currY);
}

void Pen::fillPixel(QImage* imagePtr, int currX, int currY)
{
    if (currX < imagePtr->width() && currY < imagePtr->height())
        imagePtr->setPixelColor(currX, currY, mColor);
}

