/*
  ==============================================================================

    ButtonComponent.h
    Created: 7 Jun 2023 10:32:26am
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/*
  ==============================================================================


  //// Button controls ////


  ==============================================================================
*/

//==============================================================================


class ButtonComponent  : public juce::Component,
                         public juce::Button::Listener
{
public:
    ButtonComponent(AmiSamplerAudioProcessor&);
    ~ButtonComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool& wasDiskOpClicked() { return diskOpClicked; }

private:

    /* Button commands */
    void buttonClicked(juce::Button* button) override;
    void setNumVoiceState(int state);

    /* Image buttons */
    juce::Image mTrashOff, mTrashClicked;
    juce::ImageButton mClearSample;

    /* Text Buttons */
    juce::TextButton mLoadSample{ "LOAD" }, mEnableLoop{ "LOOP" }, mSaveSample{ "SAVE" }, mSampleBacking{ " " },
                     mModelType{ "A1200" }, mLEDFilter{ "LED" }, mMonoOn{ "MEDPOLY" }, mStereoOn{ "PAULA STEREO" },
                     mStartBacking{ " " }, mEndBacking{ " " }, mReplenBacking{ " " };

    int numVoiceState{ 0 };
    bool stereoState{ false };
    bool diskOpClicked{ false };

    AmiSamplerAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonComponent)
};
