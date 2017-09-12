/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#include "view.h"
#include "ui_view.h"

View::View(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::View)
{
    // Setup items
    ui->setupUi(this);
    mAnimation.setup(ui->spriteView->SS.getImages());
    SSPtr = &(ui->spriteView->SS);
    ui->F0->setStyleSheet("border: 2px solid yellow");
    ui->F0Text->setText("Frame 1");
    this->setFocus();
    colorPrev = QImage(268, 16, QImage::Format_ARGB32);
    colorPrev.fill(ui->spriteView->SS.pen.getColor());
    ui->colorPreview->setPixmap(QPixmap::fromImage(colorPrev));
    this->setTitle();
    fileCancelled = false;

    // Pen mode connections
    connect(ui->penButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(setPen()));
    connect(ui->eraserButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(setEraser()));
    connect(ui->eyedropperButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(setEyedropper()));
    connect(ui->mirrorXaxis, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(setMirrorX()));
    connect(ui->mirrorYaxis, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(setMirrorY()));
    connect(ui->fillButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(fill()));
    connect(ui->textButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(text()));
    connect(ui->clearButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(clear()));
    connect(ui->clearButton, SIGNAL(pressed()), ui->spriteView, SLOT(render()));
    connect(ui->pixelSizeSlider, SIGNAL(valueChanged(int)), &(ui->spriteView->SS), SLOT(updatePenSize(int)));

    // Pan mode connections
    connect(&(ui->spriteView->SS.pen), SIGNAL(panBy(int, int)), ui->spriteView, SLOT(panBy(int, int)));
    connect(ui->pan, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(pan()));
    connect(ui->zoomOut, SIGNAL(pressed()), ui->spriteView, SLOT(zoomOut()));
    connect(ui->zoomIn, SIGNAL(pressed()), ui->spriteView, SLOT(zoomIn()));

    // Shape connections
    connect(ui->rectangleShapeButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(drawRectangle()));
    connect(ui->circleShapeButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(drawCircle()));
    connect(ui->lineShapeButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(drawLine()));

    // Flipping connections
    connect(ui->flipHorizontalButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(flipX()));
    connect(ui->flipVerticalButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(flipY()));

    // Shifting and rotation connections
    connect(ui->rotateLeftButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(rotateLeft()));
    connect(ui->rotateRightButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(rotateRight()));
    connect(ui->shiftUpButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(shiftUp()));
    connect(ui->shiftDownButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(shiftDown()));
    connect(ui->shiftLeftButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(shiftLeft()));
    connect(ui->shiftRightButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(shiftRight()));

    // Animation connections
    connect(&mAnimation, SIGNAL(displayImage(QImage)), this, SLOT(updatePreviewLabel(QImage)));
    connect(this, SIGNAL(startAnimation(int)), &mAnimation, SLOT(play(int)));
    connect(&mAnimation, SIGNAL(animationStatus(QString)), this, SLOT(updatePreviewButton(QString)));
    connect(this, SIGNAL(resetAll()), &mAnimation, SLOT(reset()));
    connect(ui->playbackStopPlayButton, SIGNAL(pressed()), this, SLOT(getFPS()));
    connect(ui->playbackFPSSelector, SIGNAL(valueChanged(int)), &mAnimation, SLOT(updateTimer(int)));
    connect(&(ui->spriteView->SS), SIGNAL(resetAnimation()), &mAnimation, SLOT(reset()));
    connect(this, SIGNAL(pauseAnimation()), &mAnimation, SLOT(pause()));
    connect(this, SIGNAL(resetAnimation()), &mAnimation, SLOT(pause()));
    connect(&(ui->spriteView->SS), SIGNAL(stopAnimation()), this, SLOT(getFPS()));

    // Text dialog box connections
    connect(&(ui->spriteView->SS.pen), SIGNAL(showPopup()), this, SLOT(showDialog()));


    mAnimation.setup(ui->spriteView->SS.getImages());
    connect(&popup, SIGNAL(tellViewToClose(std::string)), this, SLOT(respondToClose(std::string)));

    // Menu Item Action connections
    connect(ui->actionNewSprite, SIGNAL(triggered()), this, SLOT (newSprite()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(this, SIGNAL(fileLoad(QString)), &(ui->spriteView->SS), SLOT(load(QString)));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(fileClose()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(this, SIGNAL(fileSave(QString)), &(ui->spriteView->SS), SLOT(save(QString)));
    connect(ui->actionExport, SIGNAL(triggered()), this, SLOT(fileExport()));
    connect(this, SIGNAL(exportGIF(QString)), &(ui->spriteView->SS), SLOT(downloadGIF(QString)));
    connect(ui->actionPNG,SIGNAL(triggered(bool)),this,SLOT(PngExport()));
    connect(this, SIGNAL(exportPNG(QString)),&(ui->spriteView->SS), SLOT(downloadPNG(QString)));
    connect(&(ui->spriteView->SS), SIGNAL(selectedTool(QString)), this, SLOT(updateCurrentTool(QString)));
    connect(ui->actionUndoCtrlZ, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(undo()));
    connect(ui->actionRedoCtrlY, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(redo()));
    connect(ui->actionNewFrameCtrlF, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(newFrame()));
    connect(ui->actionNewFrameCtrlF, SIGNAL(triggered()), ui->spriteView, SLOT(render()));
    connect(ui->actionNewFrameCtrlF, SIGNAL(triggered()), this, SLOT(updateFramesPreviewBox()));
    connect(ui->actionPen, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(setPen()));
    connect(ui->actionFill, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(fill()));
    connect(ui->actionEraser, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(setEraser()));
    connect(ui->actionClear, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(clear()));
    connect(ui->actionClear, SIGNAL(triggered()), ui->spriteView, SLOT(render()));
    connect(ui->actionText, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(text()));
    connect(ui->actionEyedropper, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(setEyedropper()));
    connect(ui->actionMirrorXAxis, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(setMirrorX()));
    connect(ui->actionMirrorYAxis, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(setMirrorY()));
    connect(ui->actionZoomin, SIGNAL(triggered()), ui->spriteView, SLOT(zoomIn()));
    connect(ui->actionZoomout, SIGNAL(triggered()), ui->spriteView, SLOT(zoomOut()));
    connect(ui->actionPan, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(pan()));
    connect(ui->actionLine, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(drawLine()));
    connect(ui->actionRectangle, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(drawRectangle()));
    connect(ui->actionCircle, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(drawCircle()));
    connect(ui->actionRotateLeft, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(rotateLeft()));
    connect(ui->actionRotateRight, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(rotateRight()));
    connect(ui->actionFlipXAxis, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(flipX()));
    connect(ui->actionFlipYAxis, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(flipY()));
    connect(ui->actionShiftUp, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(shiftUp()));
    connect(ui->actionShiftDown, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(shiftDown()));
    connect(ui->actionShiftLeft, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(shiftLeft()));
    connect(ui->actionShiftRight, SIGNAL(triggered()), &(ui->spriteView->SS), SLOT(shiftRight()));


    // Update connections
    connect(this, SIGNAL(colorChosen(QColor)), &(ui->spriteView->SS), SLOT(setPenColor(QColor)));
    connect(&(ui->spriteView->SS), SIGNAL(updateAllViews()), ui->spriteView, SLOT(render()));
    connect(&(ui->spriteView->SS), SIGNAL(updateAllViews()), this, SLOT(clearPreviewFrames()));
    connect(this, SIGNAL(updateViews()), ui->spriteView, SLOT(render()));
    connect(&(ui->spriteView->SS), SIGNAL(updateViewRequest(QImage)), ui->spriteView, SLOT(updateView(QImage)));
    connect(&(ui->spriteView->SS.pen), SIGNAL(updateColorDisplay(QColor)), this, SLOT(updateFromEyedrop(QColor)));

    //Frame preview connections
    connect(ui->newSpriteButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(newFrame()));
    connect(ui->newSpriteButton, SIGNAL(pressed()), ui->spriteView, SLOT(render()));
    connect(ui->newSpriteButton, SIGNAL(pressed()), this, SLOT(updateFramesPreviewBox()));
    connect(ui->deleteSpriteButton, SIGNAL(pressed()), this, SLOT(deleteFrame()));
    connect(this, SIGNAL(deleteFrameSS()), &(ui->spriteView->SS), SLOT(deleteFrame()));
    connect(this, SIGNAL(restartAnimation()), &mAnimation, SLOT(restart()));
    connect(ui->DuplicateSpriteButton, SIGNAL(pressed()), &(ui->spriteView->SS), SLOT(copyFrame()));
    connect(ui->DuplicateSpriteButton, SIGNAL(pressed()), ui->spriteView, SLOT(render()));
    connect(ui->spriteView, SIGNAL(updateSignal()), this, SLOT(updateFramesPreviewBox()));
    connect(ui->F0, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));
    connect(ui->F1, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));
    connect(ui->F2, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));
    connect(ui->F3, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));
    connect(ui->F4, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));
    connect(ui->F5, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));
    connect(ui->F6, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));
    connect(ui->F7, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));
    connect(ui->F8, SIGNAL(clicked(QString)), this, SLOT(frameClick(QString)));

    //Onion Skin connections
    connect(ui->onionSkinCheckBox, SIGNAL(clicked(bool)), ui->spriteView, SLOT(setOnionSkin(bool)));

    //Sprite size connections
    connect(this,SIGNAL(changeImageSize(QSize)), &(ui->spriteView->SS),SLOT(setDimensions(QSize)));
    connect(this,SIGNAL(changeImageSize(QSize)), ui->spriteView,SLOT(setImageSize(QSize)));

    int  a = this->getImageSize();
    emit changeImageSize(QSize(a,a));
}

View::~View()
{
    delete ui;
}

/* Creates a file open dialog, gets the filename, and opens the file.
 * If there are existing frames, the opened file becomes the current frame.  All other frames
 * are pushed into the list.*/
void View::fileOpen()
{
    filename = QFileDialog::getOpenFileName(this, tr("Open File"), "/untitiled.ssp", tr("Images (*.ssp)"));

    //Qietly returns if the cancel button is pressed
    if (filename == "")
    {
        return;
    }

    //If there are any current frames, get those frames and store them before opening a new file
    if(ui->spriteView->SS.returnModified() || ui->spriteView->SS.getImages()->size() > 1)
    {
        //Get the current frames, iterate through them and copy them
        QVector<QImage> *imagePtr = ui->spriteView->SS.getImages();
        QVector<QImage> ::iterator it;
        QVector<QImage> images;
        for(it = imagePtr->begin(); it != imagePtr->end(); it++)
        {

            images.push_back(it->copy());
        }

        //Now reset the current image, load the new image and push all the prior images into the list
        reset();
        emit fileLoad(filename);
        for(it = images.begin(); it != images.end(); it++)
        {
            ui->spriteView->SS.addImage(it->copy());
        }
    }

    //If this is a new session, just open the file
    else
    {
         emit fileLoad(filename);
    }

    //Update all the views after loading the images
    emit updateViews();
}

/* Saves the current image(s) as a *.ssp file.*/
void View::fileSave()
{

    //Get the filename to save the document with if there isn't already a filename
    if (filename.trimmed().isEmpty())
    {
        filename = QFileDialog::getSaveFileName(this, tr("Save File"), "/untitled.ssp", tr("Images (*.ssp)"));

        //Quiety return if the cancel button is pressed
        if (filename=="")
            return;
    }

    emit fileSave(filename);
}


/* Closes the current file.  If it is modified, prompts the user to either save or discard */
void View::fileClose()
{
    if(ui->spriteView->SS.returnModified())
    {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret)
        {
          case QMessageBox::Save:
              fileSave();
              break;
          case QMessageBox::Discard:
              break;
          case QMessageBox::Cancel:
              fileCancelled = true;
              return;
          default:
              // should never be reached
              break;
        }
    }

    //Update the viewing areas.
    reset();
    updatePreviewLabel(ui->spriteView->SS.getFrame());
}

void View::PngExport()
{
    filename = QFileDialog::getSaveFileName(this, tr("Export PNG"), "/untitled.png", tr("PNG Files (*.png)"));

    emit exportPNG(filename);
}


/*  Exports the current file as a *.gif file*/
void View::fileExport()
{
    filename = QFileDialog::getSaveFileName(this, tr("Export GIF"), "/untitled.gif", tr("GIF Files (*.gif)"));

    emit exportGIF(filename);
}

/* Updates the GUI to display the current tool selected by the user*/
void View::updateCurrentTool(QString s)
{
    this->ui->currentToolLabel->setText("Current Tool: "+ s );
}

/* Updates the view with the color given*/
void View::updateFromEyedrop(QColor color)
{
    QPalette *palette1 = new QPalette();
    palette1->setColor(QPalette::ButtonText, color);
    ui->pickColorPushButton->setPalette(*palette1);
    colorPrev.fill(color);
    ui->colorPreview->setPixmap(QPixmap::fromImage(colorPrev));
}


/**This method will open a QColorDialog box to allow the user to select
 * a color and alpha value.*/
void View::on_pickColorPushButton_clicked()
{
    //Open a QColorDialog, and save the color that the user selects the RGB alpha values
    QColor color = QColorDialog::getColor(Qt::white,
                                          this,
                                          "Select Pixel Color",
                                          QColorDialog::ShowAlphaChannel
                                          | QColorDialog::DontUseNativeDialog);

    //If the color selected was valid, emit the signal to update the color in the model.
    if(color.isValid())
    {
        QPalette *palette1 = new QPalette();
        palette1->setColor(QPalette::ButtonText, color);
        ui->pickColorPushButton->setPalette(*palette1);
        emit this->colorChosen(color);
        colorPrev.fill(color);
        ui->colorPreview->setPixmap(QPixmap::fromImage(colorPrev));
    }
}

/*  Displays the dialog box for text entry. Called after the Text mode
    has been selected AND a pixel has been pressed. */
void View::showDialog()
{
    popup.show();
}

/*  Catches the signal emitted from the dialog class upon dialog closing.
    Coordinates pen interaction with the main drawing area.*/
void View::respondToClose(std::string input)
{
    SpriteSheet * SS = &(ui->spriteView->SS);
    QVector<QImage>  * images = SS->getImages();
    QImage * image = &(images->operator[](ui->spriteView->SS.getCurrentFrame()));
    ui->spriteView->SS.pen.addText(image, input);
    ui->spriteView->render();
}


/* Updates the animation preview image
 * The image is scaled up for better visibility*/
void View::updatePreviewLabel(QImage i)
{
    ui->playbackLabel_2->setPixmap(QPixmap::fromImage(i.scaled(QSize(128,128))));
    ui->playbackLabel_2->show();
}

/* Updates the animation button when it has been pressed.
 * Toggles between play and stop.*/
void View::updatePreviewButton(QString s)
{

        if(s == "Stop")
        {
            QPixmap pix(":/images/stop.png");
            QIcon icon(pix);
            ui->playbackStopPlayButton->setIcon(icon);
        }
        else
        {
            QPixmap pix(":/images/play.png");
            QIcon icon(pix);
            ui->playbackStopPlayButton->setIcon(icon);
        }
}

/* Gets the current frames per second shown in the LCD box on the GUI
 * and sends a signal containing the value.*/
void View::getFPS()
{
    emit startAnimation(ui->fpsDisplay->intValue());
}

/* Clears the viewing area.
 * If there is a current file that has not been saved,
 * a dailog appears prompting the user to save or discard.*/
void View::newSprite()
{
    if(ui->spriteView->SS.returnModified())
    {
        fileClose();
    }

    if (!fileCancelled)
    {
        reset();
        int  a = this->getImageSize();
        emit changeImageSize(QSize(a, a));
    }
}

/*  Updates the current image(s) displayed in the frame preview*/
void View::frameClick(QString frame)
{
    if(frame.at(1).digitValue() < (ui->spriteView->SS.getImages())->length())
    {
        ui->spriteView->SS.setCurrentFrame(frame.at(1).digitValue());
    }
    ui->spriteView->render();
}

/*  Deletes the frame that is currently selected in the frame preview area*/
void View::deleteFrame()
{
    if(mAnimation.isPlaying())
    {
        emit pauseAnimation();
    }

    emit deleteFrameSS();

    if(mAnimation.isPlaying())
    {
        emit restartAnimation();
    }
}

/* Updates the images in the frame preview area*/
void View::updateFramesPreviewBox()
{
    SpriteSheet * SS = &(ui->spriteView->SS);
    QVector<QImage>  * images = SS->getImages();
    int num = images->length();
    if(num >= 1)
    {
        ui->F0Text->setText("Frame 1");
        if (ui->spriteView->SS.getCurrentFrame() == 0)
        {
            ui->F0->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F0->setStyleSheet("border: 1px solid");
        }
        ui->F0->setPixmap(QPixmap::fromImage(images->at(0)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F0->clear();
    }

    if(num >= 2)
    {
        ui->F1Text->setText("Frame 2");
        if (ui->spriteView->SS.getCurrentFrame() == 1)
        {
            ui->F1->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F1->setStyleSheet("border: 1px solid");
        }
        ui->F1->setPixmap(QPixmap::fromImage(images->at(1)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F1Text->setText("");
        ui->F1->setStyleSheet("border: 0px solid");
        ui->F1->clear();
    }

    if(num >= 3)
    {
        ui->F2Text->setText("Frame 3");
        if (ui->spriteView->SS.getCurrentFrame() == 2)
        {
            ui->F2->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F2->setStyleSheet("border: 1px solid");
        }
        ui->F2->setPixmap(QPixmap::fromImage(images->at(2)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F2Text->setText("");
        ui->F2->setStyleSheet("border: 0px solid");
        ui->F2->clear();
    }

    if(num >= 4)
    {
        ui->F3Text->setText("Frame 4");
        if (ui->spriteView->SS.getCurrentFrame() == 3)
        {
            ui->F3->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F3->setStyleSheet("border: 1px solid");
        }
        ui->F3->setPixmap(QPixmap::fromImage(images->at(3)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F3Text->setText("");
        ui->F3->setStyleSheet("border: 0px solid");
        ui->F3->clear();
    }


    if(num >= 5)
    {
        ui->F4Text->setText("Frame 5");
        if (ui->spriteView->SS.getCurrentFrame() == 4)
        {
            ui->F4->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F4->setStyleSheet("border: 1px solid");
        }
        ui->F4->setPixmap(QPixmap::fromImage(images->at(4)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F4Text->setText("");
        ui->F4->setStyleSheet("border: 0px solid");
        ui->F4->clear();
    }


    if(num >= 6)
    {
        ui->F5Text->setText("Frame 6");
        if (ui->spriteView->SS.getCurrentFrame() == 5)
        {
            ui->F5->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F5->setStyleSheet("border: 1px solid");
        }
        ui->F5->setPixmap(QPixmap::fromImage(images->at(5)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F5Text->setText("");
        ui->F5->setStyleSheet("border: 0px solid");
        ui->F5->clear();
    }


    if(num >= 7)
    {
        ui->F6Text->setText("Frame 7");
        if (ui->spriteView->SS.getCurrentFrame() == 6)
        {
            ui->F6->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F6->setStyleSheet("border: 1px solid");
        }
        ui->F6->setPixmap(QPixmap::fromImage(images->at(6)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F6Text->setText("");
        ui->F6->setStyleSheet("border: 0px solid");
        ui->F6->clear();
    }


    if(num >= 8)
    {
        ui->F7Text->setText("Frame 8");
        if (ui->spriteView->SS.getCurrentFrame() == 7)
        {
            ui->F7->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F7->setStyleSheet("border: 1px solid");
        }
        ui->F7->setPixmap(QPixmap::fromImage(images->at(7)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F7Text->setText("");
        ui->F7->setStyleSheet("border: 0px solid");
        ui->F7->clear();
    }


    if(num >= 9)
    {
        ui->F8Text->setText("Frame 9");
        if (ui->spriteView->SS.getCurrentFrame() == 8)
        {
            ui->F8->setStyleSheet("border: 2px solid yellow");
        }
        else
        {
            ui->F8->setStyleSheet("border: 1px solid");
        }
        ui->F8->setPixmap(QPixmap::fromImage(images->at(8)).scaled(QSize(61, 61), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    }
    else
    {
        ui->F8Text->setText("");
        ui->F8->setStyleSheet("border: 0px solid");
        ui->F8->clear();
    }

}

/*  Clears the images displayed in the frame preview area*/
void View::clearPreviewFrames()
{
    ui->F0->clear();
    ui->F1->clear();
    ui->F2->clear();
    ui->F3->clear();
    ui->F4->clear();
    ui->F5->clear();
    ui->F6->clear();
    ui->F7->clear();
    ui->F8->clear();

    ui->F1->setStyleSheet("border: 0px solid");
    ui->F2->setStyleSheet("border: 0px solid");
    ui->F3->setStyleSheet("border: 0px solid");
    ui->F4->setStyleSheet("border: 0px solid");
    ui->F5->setStyleSheet("border: 0px solid");
    ui->F6->setStyleSheet("border: 0px solid");
    ui->F7->setStyleSheet("border: 0px solid");
    ui->F8->setStyleSheet("border: 0px solid");

    ui->F1Text->setText("");
    ui->F2Text->setText("");
    ui->F3Text->setText("");
    ui->F4Text->setText("");
    ui->F5Text->setText("");
    ui->F6Text->setText("");
    ui->F7Text->setText("");
    ui->F8Text->setText("");

}

/*Provides methods for key press events associated with menu items*/
void View::keyPressEvent(QKeyEvent *event)
{
    //Check to see if the ctrl (command on mac) modifiers have been pressed.
    if(QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        //If it has, check some cases and perform actions.
        switch(event->key())
        {
            case Qt::Key_Q:
                this->fileClose();
                exit(0);
                break;
            case Qt::Key_S:
                this->fileSave();
                break;
            case Qt::Key_N:
                this->newSprite();
                break;
            case Qt::Key_E:
                this->fileExport();
                break;
            case Qt::Key_O:
                this->fileOpen();
                break;
            case Qt::Key_W:
                this->fileClose();
                break;
            case Qt::Key_C:
                emit this->ui->DuplicateSpriteButton->pressed();
                break;
            case Qt::Key_D:
                this->deleteFrame();
                break;
            case Qt::Key_F:
                emit this->ui->newSpriteButton->pressed();
                break;
            case Qt::Key_Z:
                emit this->ui->actionUndoCtrlZ->triggered();
                break;
            case Qt::Key_Y:
                emit this->ui->actionRedoCtrlY->triggered();
                break;
           case Qt::Key_P:
                emit this->ui->actionPNG->triggered();
                break;

        }
    }
    else
    {
        switch(event->key())
        {
            case Qt::Key_W:
             emit ui->shiftUpButton->pressed();
             break;
            case Qt::Key_A:
             emit ui->shiftLeftButton->pressed();
             break;
            case Qt::Key_S:
             emit ui->shiftDownButton->pressed();
             break;
            case Qt::Key_D:
             emit ui->shiftRightButton->pressed();
             break;
            case Qt::Key_R:
             emit ui->rotateRightButton->pressed();
             break;
            case Qt::Key_L:
             emit ui->rotateLeftButton->pressed();
             break;
            case Qt::Key_Q:
             emit ui->rotateLeftButton->pressed();
             break;
            case Qt::Key_E:
             emit ui->rotateRightButton->pressed();
             break;

        }
    }
}

/* Resets the viewing areas*/
void View::reset()
{
    filename.clear();
    ui->spriteView->SS.reset();
    ui->spriteView->render();
    emit resetAll();
}

/* Creates the title "JPixel"*/
void View::setTitle()
{
    QFont font = ui->label->font();
    font.setItalic(true);
    font.setBold(true);
    font.setPointSize(72);
    font.setLetterSpacing(QFont::AbsoluteSpacing,20);
    font.setStyleHint(QFont::Cursive,QFont::PreferDevice);
    QPalette palette = ui->label->palette();
    palette.setColor(QPalette::WindowText, Qt::blue);

    ui->label->setPalette(palette);
    ui->label->setFont(font);

}

/* Returns the size of the image*/
int View::getImageSize()
{

    QStringList items;
    items <<  tr("16x16") << tr("32x32") << tr("64x64");

    bool ok;
    QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                         tr("Please pick your sprite dimensions:"), items, 0, false, &ok);
    if(!ok)
    {
        exit(1);
    }
    else
    {
        return std::stoi(item.toStdString().substr(0, 2));
    }
}
