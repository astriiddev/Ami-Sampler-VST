/*
  ==============================================================================

    WaveThumbnail.h
    Created: 22 Jan 2023 6:07:35pm
    Author:  finle

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/*
  ==============================================================================


  //// Waveform drawing and Start/End loop slider drawing ////


  ==============================================================================
*/

//==============================================================================
class WaveThumbnail : public juce::Component,
                              public juce::FileDragAndDropTarget,
                              public juce::Slider::Listener,
                              public juce::MouseListener
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

    /* Mouse listener functions */
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
    /*???? Possible TODO: loop point graphics //could// be put into OtherLookAndFeel and //
      shared with drawLinearSlider function to need fewer files, but giving loop point sliders //
      their own drawLinearSlider makes it easier to edit their graphics on the fly quicker ????*/
    juce::Slider mLoopPoints;

    float mCentreWave = 0.5f;
    float mScaleFactor = 1.0f;
    bool mOnLoopPoint = false;


    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveThumbnail)
};