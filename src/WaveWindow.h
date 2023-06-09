/*
  ==============================================================================

    WaveWindow.h
    Created: 19 May 2023 10:08:47pm
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class WaveWindow  : public juce::Component
{
public:
    WaveWindow(AmiSamplerAudioProcessor& p);
    ~WaveWindow() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    /* Windows loads custom font in paint() just fine but macOS will not,
        so an additional font function is needed for cross compatibility */
    void setFont(const juce::Font newFont) { theFont = newFont; }
    
private:

    juce::Font theFont;
    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveWindow)
};
