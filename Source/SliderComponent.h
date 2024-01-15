/*
  ==============================================================================

    SliderComponent.h
    Created: 23 Dec 2023 11:49:24am
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class SliderComponent  : public juce::Component,
                         public juce::AudioProcessorValueTreeState::Listener
{
public:
    SliderComponent(AmiAudioProcessor&);
    ~SliderComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void initSliders();
    void initSlider(juce::Slider*, juce::Label*, const juce::String);
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    int currentSample = 0, lastSample = -1;

    juce::Slider masterVolSlider,  masterPanSlider,
                 vibeSpeedSlider, vibeIntstySlider,
                 channelVolSlider, channelPanSlider,
                 downSampleSlider, glissandoSlider, fineTuneSlider,
                 attackSlider, decaySlider, sustainSlider, releaseSlider;

    juce::Label masterVolLabel,  masterPanLabel,
                vibeSpeedLabel, vibeIntstyLabel,
                channelVolLabel, channelPanLabel,
                downSampleLabel, glissandoLabel, fineTuneLabel,
                attackLabel, decayLabel, sustainLabel, releaseLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolAttachment,  masterPanAttachment,
                                                                          vibeSpeedAttachment, vibeIntstyAttachment,
                                                                          channelVolAttachment, channelPanAttachment,
                                                                          downSampleAttachment, glissandoAttachment, fineTuneAttachment,
                                                                          attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;

    AmiAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderComponent)
};
