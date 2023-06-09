/*
  ==============================================================================

    WaveThumbnail.h
    Created: 22 Jan 2023 6:07:35pm
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "OtherLookAndFeel.h"

/*
  ==============================================================================


  //// Waveform drawing and Start/End loop slider drawing ////


  ==============================================================================
*/

//==============================================================================
class WaveThumbnail : public juce::Component,
                              public juce::FileDragAndDropTarget,
                              public juce::Slider::Listener

{
public:
    WaveThumbnail(AmiSamplerAudioProcessor& p);
    ~WaveThumbnail() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /* File loading from file drag */
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    /* Slider value listenener */
    void sliderValueChanged(juce::Slider* slider) override;
    void setStartAndEndLoop();

    /* Mouse functions */
    bool hitTest(int x, int y) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    /* Scale factor value and center focus value to be called by other components */
    float& getScaleFactor() { return mScaleFactor; }
    float& getCentrePoint() { return mCentreWave; }

    /* Loop point values to be called by other components */
    juce::Slider& getLoopPoints() { return mLoopPoints; }

private:

    /* Sample values to be drawn in waveform */
    std::vector<float> mAudioPoints;

    /* Two value slider and custom graphics for two value slider */
    juce::Slider mLoopPoints;
    juce::Path minLoopFlag, maxLoopFlag;

    float mCentreWave = 0.0f;
    float mScaleFactor = 1.0f;
    bool mOnLoopPoint = false;

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveThumbnail)
};