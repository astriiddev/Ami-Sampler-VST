/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveThumbnail.h"
#include "WaveWindow.h"
#include "ADSRComponent.h"
#include "ControlComponent.h"
#include "NewSamplerVoice.h"
#include "OtherLookAndFeel.h"

/*
  ==============================================================================


  //// Top level GUI component that all GUI component roads must lead to ////


  ==============================================================================
*/

//==============================================================================
class AmiSamplerAudioProcessorEditor  : public juce::AudioProcessorEditor

{
public:
    AmiSamplerAudioProcessorEditor (AmiSamplerAudioProcessor&p);
    ~AmiSamplerAudioProcessorEditor() override;

    //=================================modal=============================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    /* GUI component initialization */
    WaveThumbnail mWaveThumbnail;
    WaveWindow mAmiWindow;
    ControlComponent mControl;
    juce::Viewport mViewWave;

    OtherLookAndFeel mLookAndFeel;

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmiSamplerAudioProcessorEditor)
};
