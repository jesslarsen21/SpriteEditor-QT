/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#include "jgraphicsview.h"

#define VIEW_SIZE 512
#define MIN_ZOOM 1.0
#define MAX_ZOOM 64.0

/* Constructor */
JGraphicsView::JGraphicsView(QWidget* parentPtr) :
    QGraphicsView(parentPtr),
    SS(QSize(32, 32), QSize(VIEW_SIZE, VIEW_SIZE)),
    mZoom(16),
    mPanXCenter(16),
    mPanYCenter(16)
{
    // Default options
    onionSkin = false;
    drawing = false;
    mImgSize = QSize(32, 32);
    QSize scaledSize = QSize(VIEW_SIZE,VIEW_SIZE);

    scene = new QGraphicsScene(this);
    this->setScene(this->scene);

    // Set up image
    image = QImage(mImgSize, QImage::Format_RGB32);
    image.fill(Qt::GlobalColor::white);

    // Set up the background "checkerboard" image
    for (int x = 0; x < mImgSize.height(); x++)
    {
        for (int y = 0; y < mImgSize.width(); y+=2)
        {
            if ((x % 2) == 0)
            {
                image.setPixel(x, y, qRgba(230, 230, 230,0));
                image.setPixel(x, y+1, qRgba(255, 255, 255,0));
            }
            else
            {
                image.setPixel(x, y+1, qRgba(230, 230, 230, 0));
                image.setPixel(x, y, qRgba(255, 255, 255, 0));
            }
        }
    }
    scene->addPixmap(QPixmap::fromImage(image.scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
    scene->addPixmap(QPixmap::fromImage(SS.getFrame().scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
    connect(&SS,SIGNAL(updateImageSize(QSize)),this,SLOT(setImageSize(QSize)));
}

JGraphicsView::~JGraphicsView()
{
    delete scene;
}

/* Transformation from screen space to draw space.*/
QPoint JGraphicsView::screenSpaceToDrawSpace(QPoint loc)
{
    // convert our screen-space point into image-space.
    double x = mPanXCenter + (loc.x()-VIEW_SIZE/2)/mZoom;
    double y = mPanYCenter + (loc.y()-VIEW_SIZE/2)/mZoom;
    return QPoint(x, y);
}


/* Handles mouse press events.*/
void JGraphicsView::mousePressEvent(QMouseEvent *eventPtr)
{
    drawing = true;
    QPoint loc = eventPtr->pos();
    loc = screenSpaceToDrawSpace(loc);
    SS.beginDraw(loc.x(), loc.y());
    render();
}


/* Handles mouse release events.*/
void JGraphicsView::mouseReleaseEvent(QMouseEvent *eventPtr)
{
    if (drawing)
    {
        QPoint loc = eventPtr->pos();
        loc = screenSpaceToDrawSpace(loc);

        drawing = false;
        SS.endDraw(loc.x(), loc.y());
        render();
    }
}

/* Handles mouse move events.*/
void JGraphicsView::mouseMoveEvent(QMouseEvent *eventPtr)
{
    if (drawing)
    {
        QPoint loc = eventPtr->pos();
        loc = screenSpaceToDrawSpace(loc);

        SS.draw(loc.x(), loc.y());
        render();
    }
}

/* Renders the scene*/
void JGraphicsView::render()
{
    int x0_imSpace = mPanXCenter-VIEW_SIZE/(2*mZoom);
    int y0_imSpace = mPanYCenter-VIEW_SIZE/(2*mZoom);

    scene->clear();

    // Only redraw checkers if size has changed
    if(mImgSize != image.size())
    {
        image = QImage(mImgSize, QImage::Format_RGB32);
        image.fill(Qt::GlobalColor::white);
        // Set up the background "checkerboard" image
        for (int x = 0; x < mImgSize.height(); x++)
        {
            for (int y = 0; y < mImgSize.width(); y+=2)
            {
                if ((x % 2) == 0)
                {
                    image.setPixel(x, y, qRgba(230, 230, 230,0));
                    image.setPixel(x, y+1, qRgba(255, 255, 255,0));
                }
                else
                {
                    image.setPixel(x, y+1, qRgba(230, 230, 230, 0));
                    image.setPixel(x, y, qRgba(255, 255, 255, 0));
                }
            }
        }
    }


    QRect subImg = QRect(x0_imSpace, y0_imSpace, VIEW_SIZE/mZoom, VIEW_SIZE/mZoom);

    if(x0_imSpace < 0)
    {
        subImg.setX(0);
    }
    if(y0_imSpace < 0)
    {
        subImg.setY(0);
    }

    if(subImg.width() + subImg.x() > mImgSize.width())
    {
        subImg.setWidth(mImgSize.width() - subImg.x());
    }

    if(subImg.height() + subImg.y() > mImgSize.height())
    {
        subImg.setHeight(mImgSize.height() - subImg.y());
    }

    QSize scaledSize = QSize(subImg.width() * mZoom, subImg.height() * mZoom);

    scene->addPixmap(QPixmap::fromImage(image.copy(subImg).scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::FastTransformation)));

    if (onionSkin)
    {
        QImage next = SS.getNext();
        QImage prev = SS.getPrev();
        scene->addPixmap(QPixmap::fromImage(prev.copy(subImg).scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::FastTransformation), 0));
        scene->addPixmap(QPixmap::fromImage(next.copy(subImg).scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::FastTransformation), 0));
    }

    scene->addPixmap(QPixmap::fromImage(SS.getFrame().copy(subImg).scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::FastTransformation)));

    if (SS.isShape())
    {
        scene->addPixmap(QPixmap::fromImage(SS.getTemp()->copy(subImg).scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::FastTransformation)));
    }

    // Map the top-left corner of our image from image space (0,0) to screen space, then transform the image.
    double x0 = (mPanXCenter-subImg.x())*mZoom - VIEW_SIZE/2;
    double y0 = (mPanYCenter-subImg.y())*mZoom - VIEW_SIZE/2;

    scene->setSceneRect(x0, y0, VIEW_SIZE, VIEW_SIZE);

    updateSignal();
}


/* Zooms in */
void JGraphicsView::zoomIn()
{
    // Increase zoom
    mZoom *= (2.0/1.0);
    if (mZoom > MAX_ZOOM) {
        mZoom = MAX_ZOOM;
    }

    panBoundsCheck();
    render();
}

/* Zooms out */
void JGraphicsView::zoomOut()
{
    // Decrease zoom
    mZoom *= (1.0/2.0);
    if (mZoom < MIN_ZOOM) {
        mZoom = MIN_ZOOM;
    }
    panBoundsCheck();
    render();
}

/* Pan (with the 'pan tool') */
void JGraphicsView::panBy(int x, int y)
{
    // Add panning
    mPanXCenter += x;
    mPanYCenter += y;
    panBoundsCheck();
}

/* Checks the panning bounds, so the image doesn't go too far out of bounds */
void JGraphicsView::panBoundsCheck()
{
    double sizeOfImageOnscreen = mImgSize.height() * mZoom;
    if (sizeOfImageOnscreen <= VIEW_SIZE) {
        // If our image is tiny, center it.
        mPanXCenter = mImgSize.width() / 2;
        mPanYCenter = mImgSize.height() / 2;
    }
    else
    {
        // Otherwise, make sure it doesn't go out-of-bounds
        double x0 = mPanXCenter - VIEW_SIZE/(2*mZoom);
        double y0 = mPanYCenter - VIEW_SIZE/(2*mZoom);
        double x1 = mPanXCenter + VIEW_SIZE/(2*mZoom);
        double y1 = mPanYCenter + VIEW_SIZE/(2*mZoom);

        // Check X
        if (x0 < -1)
        {
            mPanXCenter -= x0+1;
        }
        else if (x1 > mImgSize.width() + 1)
        {
            mPanXCenter += (mImgSize.width() + 1 - x1);
        }

        // Check Y
        if (y0 < -1)
        {
            mPanYCenter -= y0+1;
        }
        else if (y1 > mImgSize.height() + 1)
        {
            mPanYCenter += (mImgSize.height() + 1 - y1);
        }
    }
}

/* Add an image to the sprite sheet */
void JGraphicsView::addImage()
{
    SS.addImage(image);
}

/* Updates the view */
void JGraphicsView::updateView(QImage x)
{
    render();
}

/* Enables or disables onion skin */
void JGraphicsView::setOnionSkin(bool os)
{
    onionSkin = os;
    render();
}

/* Sets the image size*/
void JGraphicsView::setImageSize(QSize size)
{
    mImgSize = size;
    mZoom = VIEW_SIZE / size.height();
    mPanXCenter = size.width() / 2;
    mPanYCenter = size.height() / 2;
    render();
    panBoundsCheck();
    SS.reset();
}
