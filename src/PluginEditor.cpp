/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

/*
  ==============================================================================


  //// Top level GUI component that all other GUI component roads must lead to ////


  ==============================================================================
*/

//==============================================================================
AmiSamplerAudioProcessorEditor::AmiSamplerAudioProcessorEditor (AmiSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), mLookAndFeel(p), mAmiWindow(p), mWaveThumbnail(p), mControl(p), audioProcessor(p)
{
    /* Custom global graphics */
    juce::LookAndFeel::setDefaultLookAndFeel(&mLookAndFeel);

    /* GUI component initialization */
    addAndMakeVisible(mControl);
    addAndMakeVisible(mWaveThumbnail);
    addAndMakeVisible(mAmiWindow);
    addAndMakeVisible(mViewWave);

    /* Passes mouse click through AmiWindow to waveform */
    mAmiWindow.setInterceptsMouseClicks(false, true);
    mWaveThumbnail.setWantsKeyboardFocus(false);

    /* Attaches waveform window to a viewport for zooming and scrolling*/
    mViewWave.getHorizontalScrollBar().setAutoHide(false);
    mViewWave.setWantsKeyboardFocus(false);
    mViewWave.setInterceptsMouseClicks(false, true);
    mViewWave.setViewedComponent(&mWaveThumbnail, false);
    mViewWave.setViewedComponent(&mWaveThumbnail, false);
    
    /* Custom graphics for zoom scrollbar */
    mViewWave.getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::thumbColourId,juce::Colours::white);

    /* Sets initial window bounds and rescaling bounds */
    setResizable(true, false);
    setResizeLimits(480, 360, 960, 720);
    getConstrainer()->setFixedAspectRatio(1.333333f);

    setSize (640, 480);

}

AmiSamplerAudioProcessorEditor::~AmiSamplerAudioProcessorEditor()
{
    /* Dereferences scrollbar graphics */

    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void AmiSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    /* Viewport graphics and controls */

    /* Returns focus to ASCII MIDI if viewport is clicked*/
    if (mViewWave.hasKeyboardFocus(true))
    {
        mControl.grabKeyboardFocus();
    }

    /* Turns on scrollbar even when zoomed out */
    mViewWave.setScrollBarsShown(false, true, false, true);
    mViewWave.setScrollBarThickness(proportionOfHeight(0.0232f));

    /* Resets viewport zoom to full waveform if scale factor returns to 1 */
    if (mWaveThumbnail.getScaleFactor() <= 1.0f)
    {
        mWaveThumbnail.setBoundsRelative(0, 0.0f, 1.0f, 0.97f);
    }

    /* Centers zoomed waveform scrolling on waveform cursor */
    static float currentScaleFactor = 1.0f, currentCenterPoint = 0.5f;
    if (mWaveThumbnail.getScaleFactor() != currentScaleFactor || mWaveThumbnail.getCentrePoint() != currentCenterPoint)
    {
        mViewWave.setViewPositionProportionately(mWaveThumbnail.getCentrePoint(), 0.0f);
        currentScaleFactor = mWaveThumbnail.getScaleFactor();
        currentCenterPoint = mWaveThumbnail.getCentrePoint();
    }
}

void AmiSamplerAudioProcessorEditor::resized()
{
    /* Component positioning and sizing with respect to window resizing */
    mControl.setBoundsRelative(0.0, 0.0, 1.0, 1.0);
    mViewWave.setBoundsRelative(0.031, 0.041f, 0.938f, 0.35f);
    mWaveThumbnail.setBoundsRelative(0.0f, 0.0f, 1.0f, 0.97f);
    mAmiWindow.setBoundsRelative(0.0f, 0.0f, 1.0f, 0.4f);
}