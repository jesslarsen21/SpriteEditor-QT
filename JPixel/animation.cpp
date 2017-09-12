/* DM, TG, KP, JS, JL, JL, SG
 * Updated 7 Nov 2016 23:59
 * PS7 CS3505 Fall 2016
 */

#include "animation.h"

Animation::Animation()
{
    timerPtr = new QTimer(this);
    connect(timerPtr, SIGNAL(timeout()), this, SLOT(updateImage()));
    currentImageIndex = 0;
    playing = false;
}

Animation::~Animation(){
    delete timerPtr;
}

/*  Sets up the pointer to the list of images
 *  Must be called after the SS is created, so there is a valid pointer to the vector */
void Animation::setup(QVector<QImage> *vectorPtr)
{
    framePtr = vectorPtr;
}

/*  Starts the animation or stops it depending upon the boolean value playing */
void Animation::play(int fps)
{
    if(!playing)
    {
        timerPtr->start(1000/fps);
        playing = true;
        emit animationStatus("Stop");
    }
    else
    {
        timerPtr->stop();
        currentImage = framePtr->at(0);
        playing = false;
        emit animationStatus("Play");
    }
}

/*  Pauses the animation, but does not emit any signals to change the text label on the GUI */
void Animation::pause()
{
    timerPtr->stop();
}

/*  Restarts the animation from the first frame */
void Animation::restart()
{
    playing = true;
    currentImageIndex = 0;
    timerPtr->start();
}

/*  Updates the image that is shown in the preview frame */
void Animation::updateImage()
{
    // If there is nothing in the list, quietly exits
    if (framePtr->size() == 0){
        return;
    }

    emit displayImage(framePtr->at(currentImageIndex));
    currentImageIndex++;

    // wrap around
    if (currentImageIndex >= framePtr->size() && framePtr->size() !=0)
    {
        currentImageIndex = 0;
    }
}

/*  Updates the timer to reflect the updated frames per second selected by the user */
void Animation::updateTimer(int fps)
{
    if(playing)
    {
        timerPtr->start(1000/fps);
    }
}

/*  Stops the current animation and resets it to the first frame */
void Animation::reset()
{
    timerPtr->stop();
    playing = false;
    emit animationStatus("Play");
    currentImageIndex = 0;
    emit displayImage(framePtr->at(currentImageIndex));
}

/*Returns true if the animation is playing, false otherwise*/
bool Animation::isPlaying()
{
    return playing;
}


