/*
  ==============================================================================

    WaveThumbnail.cpp
    Created: 22 Jan 2023 6:07:35pm
    Author:  finle

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveThumbnail.h"

/*
  ==============================================================================


  //// Waveform drawing and Start/End loop slider drawing ////


  ==============================================================================
*/

//==============================================================================
WaveThumbnail::WaveThumbnail(AmiSamplerAudioProcessor& p) : mCursor(p), audioProcessor(p)
{
    /* Custom mouse cursor initialization */
    ////!!!! TODO: figure out why this component isn't inheriting parent mouse cursor !!!!//
    auto mouseCursor = juce::ImageCache::getFromMemory(BinaryData::amiMouseCursor_png, BinaryData::amiMouseCursor_pngSize);
    
    setMouseCursor(juce::MouseCursor(mouseCursor.rescaled(mouseCursor.getBounds().getWidth() * 4, 
                   mouseCursor.getBounds().getHeight() * 4, juce::Graphics::lowResamplingQuality), 0, 0));

    /* Two value sliders used for start and end loop points */
    mLoopPoints.setSliderStyle(juce::Slider::SliderStyle::TwoValueHorizontal);
    mLoopPoints.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mLoopPoints.setRange(0.0f, 1.0f, 0.0000001f);
    mLoopPoints.setLookAndFeel(&mCursor);
    mLoopPoints.addListener(this);
    mLoopPoints.setMouseCursor(getMouseCursor().ParentCursor);

    /* Slider intialiaztion and mouse listener for mouse dragging */
    setWantsKeyboardFocus(false);
    addAndMakeVisible(mLoopPoints);
    addMouseListener(this, true);
}

WaveThumbnail::~WaveThumbnail()
{
    /* Slider dereferencing */
    mLoopPoints.setLookAndFeel(nullptr);
    mLoopPoints.removeListener(this);
}

void WaveThumbnail::paint (juce::Graphics& g)
{
    /* Disables slider if no waveform is loaded */
    if (audioProcessor.getWaveForm().getNumSamples() <= 0)
    {
        mLoopPoints.setEnabled(false);
        mLoopPoints.setVisible(false);
    }
    else
    {
        mLoopPoints.setEnabled(true);
        mLoopPoints.setVisible(true);
        mLoopPoints.setMinAndMaxValues(audioProcessor.getSliderStart(), audioProcessor.getSliderEnd(), juce::sendNotificationAsync);
    }

    /* Sets loop points to sample values in audio file, disables and hides loop sliders if no loop metadata is found */
    if (audioProcessor.getLoopEnable())
    {
        mLoopPoints.setEnabled(true);
        mLoopPoints.setVisible(true);
    }
    else
    {
        mLoopPoints.setEnabled(false);
        mLoopPoints.setVisible(false);
    }

    /* Mapping sample values for audio file to XY points to be drawn */
    g.setColour(juce::Colours::white);

    auto waveform = audioProcessor.getWaveForm();
    if(waveform.getNumSamples()>0)

    {
        juce::Path p;
        mAudioPoints.clear();

        auto buffer = waveform.getReadPointer(0);

        for (int sample = 0; sample < waveform.getNumSamples(); sample ++)
        {
            mAudioPoints.push_back(buffer[sample]);
        }

        p.startNewSubPath(0, proportionOfHeight(0.5f));

        for (int sample = 0; sample < mAudioPoints.size(); ++sample)
        {
            auto pointX = juce::jmap<float>(sample, 0, mAudioPoints.size(),
                0, getWidth());
            auto pointY = juce::jmap<float>(mAudioPoints[sample], -1.0f, 1.0f,
                getHeight(), 0.0f);
            p.lineTo(pointX, pointY);
        }
        
        g.strokePath(p, juce::PathStrokeType(1.0f, juce::PathStrokeType::mitered,
            juce::PathStrokeType::butt));

        /* Creates playhead/trackhead that moves along with currently played note */
        //!!!! TODO: playhead movement sometimes doesn't track newest note if an older note is playing, fix this! !!!!//
        auto playHeadPosition = juce::jmap<int>(audioProcessor.getSamplePos(),
            0, audioProcessor.getWaveForm().getNumSamples(), 0, getWidth());

        if (audioProcessor.getSamplePos())

        {
            g.setColour(juce::Colours::white);
            g.drawLine(playHeadPosition, 0, playHeadPosition, getHeight(), proportionOfHeight(0.013103f));
        }
        else
            audioProcessor.setSamplePos(0);
        
    }

    /* Line drawing for zero-point of waveform */
    juce::Line<float> centerZero(juce::Point <float>(0.0f, proportionOfHeight(0.5f)), juce::Point <float>(getWidth(), proportionOfHeight(0.5f)));
    g.setColour(juce::Colour(0xfffc8a00));
    g.drawLine(centerZero, proportionOfHeight(0.01f));


    /* Draws cursor line to mouse click in waveform */
    g.setColour(juce::Colours::white);
    int cursorX = proportionOfWidth(mCentreWave);
    juce::Path cursorLine;
    cursorLine.startNewSubPath(cursorX, 0);
    cursorLine.lineTo(cursorX, getHeight());
    g.strokePath(cursorLine, juce::PathStrokeType(proportionOfHeight(0.01f), juce::PathStrokeType::mitered,
        juce::PathStrokeType::butt));
}

void WaveThumbnail::resized()
{
    /* Draws loop points to fill entire component */
    mLoopPoints.setBoundsRelative(0.0f, 0.0f, 1.0, 1.0f);
}

bool WaveThumbnail::isInterestedInFileDrag(const juce::StringArray& files)
{
    /* File type specification */
    //!!!! TODO: include 8SVX formatted .IFF files in allowed file types !!!!//
    for (auto file : files)
    {
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif") || file.contains(".raw"))
        {
            return true;
        }
    }
    return false;
}

void WaveThumbnail::filesDropped(const juce::StringArray& files, int x, int y)
{
    /* Loads allowed audio file dragged into waveform area */
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            auto myFile = std::make_unique<juce::File>(file);

            audioProcessor.loadFile(file);
        }
    }

    /* File pointer dereferencing */
    std::default_delete<juce::File>myFile;
    std::default_delete<juce::File>file;

    /* Returns zoom to completely zoomed out and sets loop point sample values from new sample */
    mScaleFactor = 1.0f;
    mLoopPoints.setMinAndMaxValues(audioProcessor.getSliderStart(), audioProcessor.getSliderEnd(), juce::sendNotificationAsync);
    setStartAndEndLoop();
    repaint();
}

void WaveThumbnail::sliderValueChanged(juce::Slider* slider)
{
    /* Sets loop point sample values to slider drag */
    if (slider == &mLoopPoints)
    {
        setStartAndEndLoop();
    }
}

void WaveThumbnail::setStartAndEndLoop()
{   
    /* Sets loop point sample values to slider position */
    audioProcessor.setLoopStart(juce::roundToInt(mLoopPoints.getMinValue() * audioProcessor.getWaveForm().getNumSamples()));
    audioProcessor.setLoopEnd(juce::roundToInt(mLoopPoints.getMaxValue() * audioProcessor.getWaveForm().getNumSamples()));
}

bool WaveThumbnail::hitTest(int x, int y)
{
    /* JUCE has no setSliderSnapsToMousePosition(false) function for two value sliders
       This stops the slider from snapping to all mouse clicks, creates a small windowed
       area around slider positions so that sliders only move when the mouse is down on 
       or directly near the slider positions */
    mOnLoopPoint = false;
    const int loopWindow = proportionOfWidth(0.0015f);

    if ((x <= int(mLoopPoints.getPositionOfValue(mLoopPoints.getMinValue())) + loopWindow && x >= int(mLoopPoints.getPositionOfValue(mLoopPoints.getMinValue())) - loopWindow ||
        x <= int(mLoopPoints.getPositionOfValue(mLoopPoints.getMaxValue())) + loopWindow && x >= int(mLoopPoints.getPositionOfValue(mLoopPoints.getMaxValue())) - loopWindow))
    {
        mLoopPoints.setInterceptsMouseClicks(true, true);
        mOnLoopPoint = true;
    }
    else
    {
        mLoopPoints.setInterceptsMouseClicks(false, false);
        mOnLoopPoint = false;
    }

    return true;
}

void WaveThumbnail::mouseDown(const juce::MouseEvent& e)
{    
    /* Sets mouse cursor on mouse click, double click if cursor should be moved to loop point */
    if ((!mOnLoopPoint && mScaleFactor == 1) || e.getNumberOfClicks() == 2)
        mCentreWave = (((float)e.getMouseDownX() / getWidth()));

    /* Moves mouse cursor to click, only repainting component when cursor is moved */
    static float currentCenter = 0.5f;
    if (mCentreWave != currentCenter)
    {
        repaint();
        currentCenter = mCentreWave;
    }   
}

void WaveThumbnail::mouseDrag(const juce::MouseEvent& e)
{
    /* Zooms waveform in and out. Click and drag down for zooming in, drag up for zooming out */
    if (e.mouseWasDraggedSinceMouseDown() && !mOnLoopPoint)
    {
        if (e.getDistanceFromDragStartY() / 10.0f > 1.0f)
            mScaleFactor += (float)e.getDistanceFromDragStartY() / 500;
        else if (e.getDistanceFromDragStartY() < 0.0f)
            mScaleFactor -= abs((float)e.getDistanceFromDragStartY()) / 100;

        setBoundsRelative(0.0f, 0.0f, mScaleFactor, 0.97f);
    }

    /* Resets scale factor so window won't zoom out further than the full waveform view */
    if (mScaleFactor <= 1.0f) { mScaleFactor = 1.0f; }
}

void WaveThumbnail::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    /* Dont want to have to click and drag for zooming? That's okay scrolling to zoom is also allowed :) */
    static float currentScaleFactor = 1.0f;
    if(mScaleFactor > 1.0f && mScaleFactor == currentScaleFactor)
    {
        mCentreWave = (float)e.getMouseDownX() / getWidth();
        currentScaleFactor = mScaleFactor;
    }
    
    /* Zoom in and out with Y scroll */
    if(wheel.deltaY != 0)
    {
        mScaleFactor += wheel.deltaY * 5.0f;
        setBoundsRelative(0.0f, 0.0f, mScaleFactor, 0.97f);
    }

    /* Scroll through waveform with X scroll */
    if (wheel.deltaX != 0)
    {
        mCentreWave -= wheel.deltaX/200;
    }

    /* Sets X and Y scrolling bounds */
    if (mScaleFactor <= 1)
        mScaleFactor = 1.0;
    if (mCentreWave <= 0)
        mCentreWave = 0.0f;
    if (mCentreWave >= 1.0f)
        mCentreWave = 1.0f;
}