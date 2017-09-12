/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#include "spritesheet.h"
#include "gif.h"


SpriteSheet::SpriteSheet(QSize size, QSize viewSize)
{
     mDimensions = size;
     mScaledSize = viewSize;
     mFrames = QVector<QImage>();
     currentFrame = 0;
     isModified = false;
     drawingShape = false;
     temp = QImage(mDimensions, QImage::Format_ARGB32);
     temp.fill(qRgba(255, 255, 255, 0));
     undoStack = new QStack<QImage>();
     redoStack = new QStack<QImage>();
     newFrame();
     undoStack->push(mFrames[currentFrame]);
}

SpriteSheet::~SpriteSheet()
{
    delete undoStack;
    delete redoStack;
}

/*Saves the QImage frames as an ASCII file*/
void SpriteSheet::save(QString filename)
{
    QFile file(filename);
    if(file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream out(&file);
        QImage image = mFrames.at(0);

        out << image.height() << " " << image.width() << "\n";

        out << mFrames.size() << "\n";

        //iterate through frames
        for (int f = 0; f < mFrames.size(); f++)
        {

            //iterate through rows
            for (int r = 0; r < image.height(); r++)
            {
                //iterate through columns
                for (int c = 0; c < image.width(); c++)
                {
                    QColor color = mFrames.at(f).pixelColor(r, c);
                    out << color.red() << " " << color.green() << " " << color.blue()<< " " << color.alpha()<< " ";
                }

                //newline between each row
                out << "\n";
            }
        }
    }

    isModified = false;
    file.close();
}

/*  Loads a *.ssp file into the sprite editor.  The *.ssp file is an ASCII text file*/
void SpriteSheet::load(QString filename)
{

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //Dialog box saying that file does not exist
        QMessageBox msgBox;
        msgBox.setText("The file cannot be opened!");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
    
    QTextStream in(&file);
    while(!in.atEnd())
    {

        //Read in the dimensions of the image
        QString line = in.readLine();
        QStringList stringList = line.split(QRegExp("\\s+"));
        int height = stringList.at(0).toInt();
        int width = stringList.at(1).toInt();
        //Update the image to reflect the proper size.
        setDimensions(QSize(height,width));
        emit updateImageSize(QSize(height,width));
        //Read in the number of frames
        line = in.readLine();
        int numFrames = line.toInt();

        //Read in each frame
        for (int i = 0; i < numFrames; i++)
        {
            QImage image(height, width, QImage::Format_ARGB32);

            //iterate through all of the rows
            for (int r = 0; r < height; r++)
            {

                //Read in each line
                line = in.readLine();
                stringList = line.split(QRegExp("\\s+"));


                //Iterate through all of the columns
                for (int c = 0; c < width; c++)
                {
                    QColor value;
                    QRgb rgbValue = qRgba(stringList.at(c*4).toInt(), stringList.at(c*4 + 1).toInt(), stringList.at(c*4 + 2).toInt(), stringList.at(c*4 + 3).toInt());
                    value.setRgba(rgbValue);
                    image.setPixelColor(r,c, value);
                }
            }

            if (i == 0)
            {
                mFrames[0] = image.copy();
            }

            else
            {
                //Add the frame to the list
                mFrames.push_back(image.copy());
            }
          }
      }

    isModified = false;
    file.close();
}

void SpriteSheet::downloadPNG(QString filename)
{
    QDir dir(QDir::currentPath());
    QString relativePath = dir.relativeFilePath(filename);
    if(QSysInfo::WV_None != QSysInfo::windowsVersion())
    {
        relativePath = relativePath.replace(QRegExp("/"), "\\");
    }
    // Create an Image large enough to fit all the frames with a 20 pixel gap between them
    QImage write((mFrames.at(0).width()*mFrames.size())+(20*(mFrames.size()-1)), mFrames.at(0).width(), QImage::Format_ARGB32);
    QPainter writer;
    writer.begin(&write);
    int t = 0;
    // go through all the frames and copy them to the image
    for(QImage img : mFrames)
    {
        writer.drawImage(t, 0, img);
        t += 20 + img.width(); // add the necessary gap to the start of the next image
    }

    // save the image
    write.save(relativePath);
}

/*Saves the current frame(s)in the sprite editor as a *.gif file*/
void SpriteSheet::downloadGIF(QString filename)
{
    //convert filename to be handled by GifBegin()
    QDir dir(QDir::currentPath());
    QString relativePath = dir.relativeFilePath(filename);
    if(QSysInfo::WV_None != QSysInfo::windowsVersion())
    {
        relativePath = relativePath.replace(QRegExp("/"), "\\");
    }
    GifWriter writer;
    GifBegin(&writer, relativePath.toStdString().c_str(), mDimensions.width(), mDimensions.height(), 250);
    for(QImage img : mFrames)
    {
        //convert frame to a uint8_t array in the RGBA8 format
        uint8_t* newImg = new uint8_t[mDimensions.width() * mDimensions.height() * 4];
        for(int i = 0; i < mDimensions.width(); i++)
        {
            for(int j = 0; j < mDimensions.height(); j++)
            {
                QColor color = img.pixelColor(j, i);
                newImg[(i * mDimensions.width() + j) * 4] = color.red();
                newImg[(i * mDimensions.width() + j) * 4 + 1] = color.green();
                newImg[(i * mDimensions.width() + j) * 4 + 2] = color.blue();
                newImg[(i * mDimensions.width() + j) * 4 + 3] = color.alpha();
            }
        }

        GifWriteFrame(&writer, newImg, mDimensions.width(), mDimensions.height(), 250);
        delete newImg;
    }
    GifEnd(&writer);
}

/* Reverses the last edit*/
void SpriteSheet::undo()
{
    isModified = true;
    if(undoStack->size() == 0)
    {
        undoStack->push(mFrames[currentFrame]);
    }

    //Check to make sure atleast 1 item is on the stack
    if(undoStack->size() > 1)
    {
        //Add current item to redo stack
        redoStack->push(undoStack->pop());
    }
    mFrames[currentFrame]=undoStack->top();

    //Ensure that the undostack at least has a blank frame
    if(undoStack->size() == 0)
    {
        undoStack->push(mFrames[currentFrame]);
    }
    //updateviewrequest
    emit updateViewRequest(mFrames[currentFrame]);
}

/* Redo the last command*/
void SpriteSheet::redo()
{
    isModified = true;
    //Make sure that the redoStack is greater then 0
    if(redoStack->size() > 0)
    {
        mFrames[currentFrame]=redoStack->pop();
        undoStack->push(mFrames[currentFrame]);
    }
    //update the view
    emit updateViewRequest(mFrames[currentFrame]);
}

/* Draws on the current image with the given coordinates*/
void SpriteSheet::draw(int x, int y)
{
    isModified = true;
    if(drawingShape)
    {
        pen.draw(&temp, x, y);
    }
    else
    {
        pen.draw(&mFrames[currentFrame], x , y);
    }
}

/* Begin drawing using the given coordinates*/
void SpriteSheet::beginDraw(int x, int y)
{
    isModified = true;
    if(drawingShape)
    {
        pen.beginDraw(&temp, x, y);
    }
    else
    {
        pen.beginDraw(&mFrames[currentFrame], x , y);
    }
}

/* End drawing using the given coordinates*/
void SpriteSheet::endDraw(int x, int y)
{
    isModified = true;
    pen.endDraw(&mFrames[currentFrame], x , y);
    temp.fill(qRgba(255, 255, 255, 0));
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
}

/*Creates a new frame for drawing*/
void SpriteSheet::newFrame()
{
      QImage image = QImage(mDimensions, QImage::Format_ARGB32);

      //set background to be transparent
      image.fill(qRgba(255, 255, 255, 0));

      if(mFrames.size() == 9)
      {
          return;
      }

      if(currentFrame == (mFrames.length() - 1) || mFrames.length() == 0)
      {
          mFrames.push_back(image.copy());
          currentFrame = (mFrames.length() - 1);

      }
      else
      {
          mFrames.insert(currentFrame + 1, image.copy());
          currentFrame++;
      }
      undoStack->clear();
      undoStack->push(mFrames[currentFrame]);
      redoStack->clear();
      emit updateViewRequest(mFrames[currentFrame]);
      isModified = true;
}

/* Deletes the current frame*/
void SpriteSheet::deleteFrame()
{
    if(mFrames.length() == 1)
    {
        clear();
        emit resetAnimation();
        updateViewRequest(mFrames[currentFrame]);
    }

    if(mFrames.length() > 1 && currentFrame == 0)
    {
        isModified = true;
        mFrames.removeAt(currentFrame);
        if(currentFrame == (mFrames.length())) currentFrame--;
        updateViewRequest(mFrames[currentFrame]);
    }
    else if(mFrames.length() > 1)
    {
        isModified = true;
        mFrames.removeAt(currentFrame);
        // if last one - currentFrame = 2 length = 3
        if(currentFrame == (mFrames.length())) currentFrame--;
        updateViewRequest(mFrames[currentFrame]);
    }

    //clear the undo/redo stacks
    undoStack->clear();
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();

    //add frame to the undo stack in error correction
    isModified = true;
}

/* Copies the current frame and adds it the frame stack*/
void SpriteSheet::copyFrame()
{
    if(mFrames.size() == 9)
    {
        return;
    }
    QImage newImage = mFrames[currentFrame].copy();
    mFrames.insert(currentFrame+1,newImage);
    currentFrame = currentFrame+1;
    undoStack->clear();
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    isModified = true;
}

/* Sets the current frame to the given frame*/
void SpriteSheet::setCurrentFrame(int frame)
{
    currentFrame = frame;
    undoStack->clear();
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
}

/* Returns the current frame*/
QImage SpriteSheet::getFrame()
{
    QImage curr = mFrames[currentFrame];
    return curr;
}

/* Returns a reference to the list of images*/
QVector<QImage>* SpriteSheet::getImages()
{
    return &mFrames;
}

/* Returns the image prior the current image*/
QImage SpriteSheet::getPrev()
{
    QImage image;
    if(currentFrame - 1 >= 0)
    {
       image =  mFrames[currentFrame - 1].copy();
       for(int i = 0; i < mDimensions.height(); i++)
       {
           for(int j = 0; j <mDimensions.width(); j ++)
           {
               QColor color = image.pixelColor(i, j);
               if(color.alpha() != 0)
               {
                   QRgb rgbValue = qRgba(100, 0, 0, 90);
                   color.setRgba(rgbValue);
                   image.setPixelColor(i,j, color);
               }
           }
       }
       return image;
    }
    image = QImage(mDimensions, QImage::Format_ARGB32);
    image.fill(qRgba(255, 255, 255, 0));
    return image;
}

/* Returns the image after the next image*/
QImage SpriteSheet::getNext()
{
    QImage image;
    if(currentFrame + 1 <= 9 && currentFrame != (mFrames.length() - 1))
    {
       image =  mFrames[currentFrame + 1].copy();
       for(int i = 0; i < mDimensions.height(); i++)
       {
           for(int j = 0; j <mDimensions.width(); j ++)
           {
               QColor color = image.pixelColor(i, j);
               if(color.alpha() != 0)
               {
                   QRgb rgbValue = qRgba(0, 0, 100, 90);
                   color.setRgba(rgbValue);
                   image.setPixelColor(i,j, color);
               }
           }
       }
       return image;
    }
    image = QImage(mDimensions, QImage::Format_ARGB32);
    image.fill(qRgba(255, 255, 255, 0));
    return image;
}

/* Updates the pen size to the given num of pixels*/
void SpriteSheet::updatePenSize(int i)
{
    this->pen.penSize(i);
}

/* Sets the drawing mode to Pen*/
void SpriteSheet::setPen()
{
    drawingShape = false;
    setPenMode(PenMode_Pencil);
    emit this->selectedTool("Pen");
}

/*Sets the drawing mode Eraser*/
void SpriteSheet::setEraser()
{
    drawingShape = false;
    setPenMode(PenMode_Eraser);
    emit this->selectedTool("Eraser");
}

/*Sets the drawing mode to Eyedropper*/
void SpriteSheet::setEyedropper()
{
    drawingShape = false;
    setPenMode(PenMode_Eyedropper);
    emit this->selectedTool("Eyedropper");
}

/* Sets the drawing mode to fill*/
void SpriteSheet::fill()
{
    setPenMode(PenMode_Fill);
    emit this->selectedTool("Fill");
}

/*Sets the drawing mode to text*/
void SpriteSheet::text()
{
    drawingShape = false;
    setPenMode(PenMode_Text);
    emit this->selectedTool("Text");
}

/* Clears the current image*/
void SpriteSheet::clear()
{
    mFrames[currentFrame].fill(qRgba(255, 255, 255, 0));
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    isModified = true;
}

/*Sets the drawing mode to pan*/
void SpriteSheet::pan()
{
    drawingShape = false;
    setPenMode(PenMode_Pan);
    emit this->selectedTool("Pan");
}

/* Sets the current pen color to the given color*/
void SpriteSheet::setPenColor(QColor c)
{
    this->pen.setColor(c);
}

/* Sets the drawing mode to MirroX.  This makes the image replicate itself in real time across the x axis*/
void SpriteSheet::setMirrorX()
{
    drawingShape = false;
    setPenMode(PenMode_MirrorXAxis);
    emit this->selectedTool("Mirror X-Axis");
}

/* Sets the drawing mode to MirroY.  This makes the image replicate itself in real time across the y axis*/
void SpriteSheet::setMirrorY()
{
    drawingShape = false;
    setPenMode(PenMode_MirrorYAxis);
    emit this->selectedTool("Mirror Y-Axis");
}

/*Sets the drawing mode to create a rectangle*/
void SpriteSheet::drawRectangle()
{
    drawingShape = true;
    setPenMode(PenMode_Rectangle);
    emit this->selectedTool("Rectangle");
}

/*Sets the drawing mode to create a circle*/
void SpriteSheet::drawCircle()
{
    drawingShape = true;
    setPenMode(PenMode_Circle);
    emit this->selectedTool("Circle");
}

/*Sets the drawing mode to create a line*/
void SpriteSheet::drawLine()
{
   drawingShape = true;
   setPenMode(PenMode_Line);
   emit this->selectedTool("Line");
}

/* Returns a mirrored image across the X axis*/
void SpriteSheet::flipX()
{
    mFrames[currentFrame] = mFrames[currentFrame].mirrored(false, true);
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    emit updateViewRequest(mFrames[currentFrame]);
    isModified = true;
}

/* Returns a mirrored image across the Y axis*/
void SpriteSheet::flipY()
{
    mFrames[currentFrame] = mFrames[currentFrame].mirrored(true,false);
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    emit updateViewRequest(mFrames[currentFrame]);
    isModified = true;
}

/* Rotates the image by 90 degrees to the right*/
void SpriteSheet::rotateRight()
{
    QTransform myTransform;
    myTransform.rotate(90);
    mFrames[currentFrame] = mFrames[currentFrame].transformed(myTransform);
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    emit updateViewRequest(mFrames[currentFrame]);
    isModified = true;
}

/*Rotates the image by 90 degrees to the left*/
void SpriteSheet::rotateLeft()
{
    QTransform myTransform;
    myTransform.rotate(-90);
    mFrames[currentFrame] = mFrames[currentFrame].transformed(myTransform);
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    emit updateViewRequest(mFrames[currentFrame]);
    isModified = true;
}

/* Shifts the image up by one pixel*/
void SpriteSheet:: shiftUp()
{
    // make a copy of myImage shifted up by one pixel
    QImage myImageX = mFrames[currentFrame].copy(0,1,mFrames[currentFrame].width(),mFrames[currentFrame].height()); // shift up

    // Set pixel i in the last row of the shifted image to pixel i of the top row
    // of the original image
    for (int i = 0; i < mFrames[currentFrame].width(); ++i)
    {
        myImageX.setPixel(i, myImageX.height()-1,mFrames[currentFrame].pixel(i,0));
    }

    // Need to update frame to the new image
    mFrames[currentFrame] = myImageX;
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    emit updateViewRequest(mFrames[currentFrame]);
    isModified = true;
}

/* Shifts the image down by one pixel*/
void SpriteSheet::shiftDown()
{
    // make a copy of myImage shifted down by one pixel
    QImage myImageX = mFrames[currentFrame].copy(0,-1,mFrames[currentFrame].width(),mFrames[currentFrame].height()); // shift down

    // Set pixel i in the top row of the shifted image to pixel i of the last row
    // of the original image
    for (int i = 0; i < mFrames[currentFrame].width(); ++i)
    {
        myImageX.setPixel(i, 0,mFrames[currentFrame].pixel(i,mFrames[currentFrame].height()-1));
    }

    // Need to update frame to the new image
    mFrames[currentFrame] = myImageX;
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    emit updateViewRequest(mFrames[currentFrame]);
    isModified = true;
}

/* Shifts the image left by one pixel*/
void SpriteSheet::shiftLeft()
{
    // make a copy of myImage shifted left by one pixel
    QImage myImageX = mFrames[currentFrame].copy(1,0,mFrames[currentFrame].width(),mFrames[currentFrame].height()); // shift left

    // Set pixel j in the last column of the shifted image to pixel j of the first column
    // of the original image
    for (int j = 0; j <mFrames[currentFrame].width(); ++j)
    {
        myImageX.setPixel(myImageX.width()-1, j,mFrames[currentFrame].pixel(0,j));
    }

    // Need to update frame to the new image
    mFrames[currentFrame] = myImageX;
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    emit updateViewRequest(mFrames[currentFrame]);
    isModified = true;
}

/* Shifts the image right by one pixel*/
void SpriteSheet::shiftRight()
{
    // make a copy of myImage shifted right by one pixel
    QImage myImageX = mFrames[currentFrame].copy(-1,0,mFrames[currentFrame].width(),mFrames[currentFrame].height()); // shift right

    for (int j = 0; j < mFrames[currentFrame].width(); ++j)
    {
        myImageX.setPixel(0, j,mFrames[currentFrame].pixel(mFrames[currentFrame].width()-1,j));
    }

    // Need to update frame to the new image
    mFrames[currentFrame] = myImageX;
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    emit updateViewRequest(mFrames[currentFrame]);
    isModified = true;
}

/*  Adds the given image to the list of images*/
void SpriteSheet::addImage(QImage image)
{
    mFrames.append(image);
    isModified = true;
}

/* Returns the number of the current frame*/
int SpriteSheet::getCurrentFrame()
{
    return currentFrame;
}

/*If the sprite sheet has been modified without saving, returns true; otherwise returns false*/
bool SpriteSheet::returnModified()
{
    return isModified;
}


QImage* SpriteSheet::getTemp()
{
    return &temp;
}

/* Returns true if the drawing mode is a shape; false otherwise*/
bool SpriteSheet::isShape()
{
    return drawingShape;
}

/*  Resets the sprite sheet.  Clears all the frames, set the current frame to 0.*/
void SpriteSheet::reset()
{
    mFrames = QVector<QImage>();
    newFrame();
    currentFrame = 0;
    undoStack->clear();
    undoStack->push(mFrames[currentFrame]);
    redoStack->clear();
    isModified = false;
    emit updateAllViews();
}

/* Private helper method that returns the coordinates of the current pixel*/
QPoint SpriteSheet::getPixelCoordinates(int x, int y)
{
    QPoint coord = QPoint(floor(x/(mScaledSize.rheight()/mDimensions.rheight())), floor(y/(mScaledSize.rwidth()/mDimensions.rwidth())));
    return coord;
}


/* Private helper method that sets the current pen mode to the given mode*/
void SpriteSheet::setPenMode(PenMode mode)
{
    pen.setMode(mode);
}

void SpriteSheet::setDimensions(QSize dim)
{
    mDimensions = dim;
    temp = QImage(dim, QImage::Format_ARGB32);
    temp.fill(QColor(255, 255, 255, 0));
}
